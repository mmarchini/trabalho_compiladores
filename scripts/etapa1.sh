if [ -d "tmp" ]; then
    rm -rf tmp;
fi
mkdir tmp;
cp Makefile tmp/;
cp analisador.l tmp/;
cp main.c tmp/;

cd tmp;
tar cvzf etapa1.tgz .;
mv etapa1.tgz ../;
cd ..;
if [ ! -d "etapas" ]; then
    mkdir etapas;
fi
if [ ! -f "etapas/etapa1.tgz" ]; then
    rm etapas/etapa1.tgz;
fi
mv etapa1.tgz etapas;
rm -rf tmp;

if [ ! -z "$1" ]; then
    rsync etapas/etapa1.tgz $1@html.inf.ufrgs.br:/home/grad/$1/public_html/etapa1.tgz;
fi


