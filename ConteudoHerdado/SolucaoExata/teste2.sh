i=0
echo "Especifique a instância:"
read inst
underline="_"
escolha="Spd_RF2_$inst$underline"
echo $escolha

# Diretórios:
#   Spd_Inst_Rid_Final2_20_500_Orig (2 2 0)
#        Spd_RF2_n_
#
#   Spd_Inst_Rid_Final2_600_1000_Orig (2 3 0)
#        Spd_RF2_n_m_

>resultados.txt

for instancia in "Instancias/Spd_Inst_Rid_Final2_20_500_Orig/"*
do
    if [[ $instancia == *$escolha* ]]; then
        #echo $instancia
        ./dmbv $instancia 2 2 0 >>resultados.txt
    fi
done

echo " "
echo "ILS      Tempo"
lua calculaMedia.lua


