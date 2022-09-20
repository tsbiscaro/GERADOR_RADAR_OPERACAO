#!/bin/bash
#
#  Thiago Biscaro - thiago.biscaro@inpe.br - 2022-01-31
#
# SCRIPT PARA CONCATENACAO DOS DADOS BRUTOS DO RADAR CEMIG - MATEUS LEME/MG
#
# - arquivo 1 tem 4 elevacoes, arquivo 2 tem 6 elevacoes
# - arquivo com 1 elevacao e' surveillance e deve ser descartado
#
# - os arquivos devem ter 12 variaveis
#
# - esse script pega os 10 ultimos arquivos, verifica qual tem 4 elevacoes
# e 12 variaveis
# - caso esses testes sejam efetuados com sucesso, verifica se o proximo
# arquivo tem 6 elevacoes e 12 variaveis
# - verifica se a diferenca de tempo entre eles e' menor que 3 minutos
# - caso positivo, gera um arquivo UF concatenando os 2 arquivos
#
# - O nome do arquivo de saida sera o mesmo nome do arquivo de entrada, com
# o sufixo _OUT.UF.gz
#
# - Os arquivos tem formato 'RME'yyMMddhhmmss.'RAW'xxx
#
# PASSO 1 - verifica se o ultimo arquivo foi processado
if [ -f ultimo ]
then
    arq=`cat ultimo`
    if [ -f "$arq" ]
    then
        echo 'Arquivo '"$arq"' ja processado'
        exit
    fi
fi

#
# PASSO 2 - varre a lista dos ultimos 10 arquivos
#
candidato1='UNDEF'
for arq in `ls -1 RME* | tail -6000`
do
    saida=`./conta_elevacoes_vars.x $arq`
    nelev=${saida:0:2}
    nvars=${saida:3:5}
    #o primeiro arquivo sempre tem 4 elevacoes
    if [ "$nelev" = 04 ] && [ "$nvars" = 12 ]
    then
        candidato1=$arq
    fi
    #o segundo tem 6
    if [ "$nelev" = 06 ] && [ "$nvars" = 12 ] && [ "$candidato1" != UNDEF ]
    then
        candidato2=$arq
        echo $candidato1
        echo $candidato2
        #verifica se a diferenca de tempo e' menor que 3 minutos
        #o #10 converte tudo pra base 10 caso um numero tenha 0 a esquerda
        dia1=$((10#${candidato1:7:2}*24*60*60))
        hora1=$((10#${candidato1:9:2}*60*60))
        minuto1=$((10#${candidato1:11:2}*60))
        segundo1=$((10#${candidato1:13:2}))
        tempo1=$(($dia1+$hora1+$minuto1+$segundo1))
        
        dia2=$((10#${candidato2:7:2}*24*60*60))
        hora2=$((10#${candidato2:9:2}*60*60))
        minuto2=$((10#${candidato2:11:2}*60))
        segundo2=$((10#${candidato2:13:2}))
        tempo2=$(($dia2+$hora2+$minuto2+$segundo2))

        if [ $(($tempo2-$tempo1)) -lt 180 ]
        then
            echo $candidato1 > ultimo
            echo $candidato1 > lista
            echo $candidato2 >> lista
            ./concatena_vol_2.x lista
        fi
    fi
done
