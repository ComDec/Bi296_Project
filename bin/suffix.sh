#!/bin/sh
FILES=`ls ./*.fq.fa`
 
for txt in $FILES;do
    doc=`echo $txt | sed "s/\.fq.fa/\.fa/"`
    mv $txt $doc
done
