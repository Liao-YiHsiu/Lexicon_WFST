#!/bin/bash

if [ $# -ne 1 ] && [ $# -ne 2 ]; then
   echo "Usage: $0 input_phone_seq_file [nbest=1]"
   echo "ex. $0 example 100"
   echo ""
   echo "Notice the number of output is not exactly nbest."
   echo "The duplicated results are merged."
   exit -1;
fi

input=$1
tmpdir=$(mktemp -d)
nbest=1
[ $# -gt 1 ] && nbest=$2

[ $input == "-" ] && input=$tmpdir/input && cat /dev/stdin > $input
[ -f $input ] || exit -1;

# check neccessary files
files="phones.60-48-39.map timitdic.txt timit_norm_trans.pl make_lexicon_fst.pl add_disambig.pl fstaddselfloops"
for f in $files;
do
   [ -f $f ] || exit -1;
done

# make programs
make > /dev/null 2> /dev/null

#generate Lexicon.fst
if [ ! -f Lexicon.fst ]; then
   cut -f3 phones.60-48-39.map | grep -v "q" | sort | uniq > phone_list

   echo "<eps> 0" > phones_num
   awk '{ print $0 " " FNR }' phone_list >> phones_num

   grep -v -E "^;" timitdic.txt | sed -e 's/\///g' -e 's/[0-9]//g' -e 's/\~[a-zA-Z]* / /g' > lexicon.txt
   echo "<s> sil" >> lexicon.txt
   echo "</s> sil" >> lexicon.txt
   echo "SIL sil" >> lexicon.txt
   paste phone_list phone_list >> lexicon.txt

   ./timit_norm_trans.pl -ignore -i lexicon.txt -m phones.60-48-39.map -from 60 -to 39 > lexicon.39.txt

   cut -f1 -d ' ' lexicon.39.txt | \
      cat - <(echo "#0") | \
      awk '{ print $0 " " FNR }' | \
      cat <(echo "<eps> 0") - > words.txt

   # add disambig
   ndisambig=`./add_lex_disambig.pl lexicon.39.txt lexicon_disambig.txt` 
   ndisambig=$[$ndisambig+1];

   ./add_disambig.pl --include-zero phones_num $ndisambig  > phones_disambig.txt 

   phone_disambig_symbol=`grep \#0 phones_disambig.txt | awk '{print $2}'`
   word_disambig_symbol=`grep \#0 words.txt | awk '{print $2}'`

   ./make_lexicon_fst.pl lexicon.39.txt 0.5 "sil" \
      | fstcompile --isymbols=phones_disambig.txt \
      --osymbols=words.txt --keep_isymbols=false --keep_osymbols=false \
      | ./fstaddselfloops  "echo $phone_disambig_symbol |" \
      "echo $word_disambig_symbol |" \
      | fstarcsort --sort_type=olabel > Lexicon.fst 
fi

# generate input.fst
#   ./timit_norm_trans.pl -i $input -m phones.60-48-39.map -from 60 -to 39 | sed -e 's/\bsil\b/ /g'| sed -e 's/  / /g' > $tmpdir/input.39 
#   
#   # read example
#   j=0; 
#   for phone in $(cat $tmpdir/input.39); 
#   do 
#      echo "$j $((j+1)) $phone $phone 0" >> $tmpdir/input.log
#      # deletion
#      #echo "$j $((j+1)) $phone <eps> 100" >> $tmpdir/input.log
#      ## substitution
#      #for tmp in $(cat phone_list | grep -w -v $phone);
#      #do
#      #   echo "$j $((j+1)) $phone $tmp 100" >> $tmpdir/input.log
#      #done
#      j=$((j+1))
#   done
#   echo "$j 0" >> $tmpdir/input.log

   ./generate $input phones.60-39.map rule.out > $tmpdir/input.log
   
   fstcompile --isymbols=phones_disambig.txt --osymbols=phones_disambig.txt $tmpdir/input.log | \
      fstarcsort --sort_type=olabel > $tmpdir/input.fst

command="fstcompose $tmpdir/input.fst Lexicon.fst | \
   fstshortestpath --nshortest=$nbest | \
   ./fstprintallpath - words.txt  "
command+=" | sed "
command+=" -e 's/<s>//g' -e 's/<\/s>//g' -e 's/SIL//g' "
command+=$(while read phone; do echo " -e \"s/\b${phone}\b/ /g\""; done < phone_list)
command+=" | tr -s ' ' | sed -e 's/^ //g' | sort | uniq"

eval $command

rm -rf $tmpdir

# use the following command to draw the fst.
# fstdraw --isymbols=phones_disambig.txt --osymbols=phones_disambig.txt -portrait input.fst | \
#   dot -Tsvg >ex.svg
exit 0;
