#!/bin/bash

outdir=ep2-thiago-wilson

if [ ! -d $outdir ]; then
  mkdir $outdir
fi

contents="src/ Makefile objs.makefile deps.makefile LEIAME"
cp -r $contents $outdir

tar -caf $outdir.tar.gz $outdir

rm -rf $outdir

