# Cria pasta tmp
if [ -d "tmp" ]; then
    rm -rf tmp;
fi
mkdir tmp;

# Copia os arquivos para a pasta tmp e entra na pasta tmp
for file in "hash.c hash.h main.c Makefile scanner.l tokens.h util.c util.h"
do
    cp $file tmp/;
    cp $file tmp/;
    cp $file tmp/;
done;
cd tmp;

# Compacta os arquivos da pasta tmp e move ele para a raiz
tar cvzf .temporary_file.tgz .;
mv .temporary_file.tgz ../etapa1.tgz ;

# Volta para a raiz e move o arquivo para a pasta etapas
cd ..;
if [ ! -d "etapas" ]; then
    mkdir etapas;
fi
if [ -f "etapas/etapa1.tgz" ]; then
    rm -f etapas/etapa1.tgz;
fi
mv etapa1.tgz etapas;

# Remove a pasta tmp
rm -rf tmp;

# Tenta sincronizar o arquivo com o servidor
if [ ! -z "$1" ]; then
    rsync etapas/etapa1.tgz $1@html.inf.ufrgs.br:/home/grad/$1/public_html/etapa1.tgz;
fi


