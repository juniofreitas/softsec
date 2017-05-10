# Transforma para XML

use Time::localtime;

# verifica se o parametro foi passado corretamente
if($#ARGV+1!=2){
   print "Parametros errados!!!\nO certo eh \"converte2.pl [ARQ][DIR.DESTINO]\"";
   exit;
}
else{
   # recebe argumento
   $arq = $ARGV[0];
   $res = $ARGV[1];
}
print "<<CRIA ARQUIVO NO FORMATO XML DO ARQUIVO DE REGISTRO PASSADO>>\n";

# abre o arquivo passado
open(XML, $arq) || die "O arquivo $arq nao pode ser aberto";
#data atual
$data = sprintf("%02d/%02d/%04d",localtime->mday(), localtime->mon(), localtime->year()+1900);
# cria o nome do arquivo de saida
$arquivox = sprintf("%s%02d%02d%02d%s","XML",localtime->hour(), localtime->min(), localtime->sec(),".xml");
# cria o arquivo de saida
open  (ARQ, ">$res/$arquivox");
#
print "Inicializando ...\n";

@texto = <XML>;
#chomp (@texto); #Retira quebra de linhas
close (XML);
#$texto = "@texto";
#$texto =~ s/\r|\n|\f//g;

print "OK!\n";
# para cada registro do arquivo
print ARQ "<root>\n";
#----------------------------------------------------------------------------------------------------------------------------
foreach $linha(@texto){

 print ARQ "\t<registro>\n";

 $i = 1;
 while ($linha =~ /\t/g){
   #$inicart = $`;
   #$inicart =~ /\./;
   $nome = $`;
   $linha = $';
   if($i == 1){
      print ARQ "\t\t<id>$nome</id>\n";
   }
   elsif($i == 2){
      print ARQ "\t\t<nome>$nome</nome>\n";
   }
   elsif($i == 3){
      print ARQ "\t\t<sobre_nome>$nome</sobre_nome>\n";
   }
   elsif($i == 4){
      print ARQ "\t\t<numero>$nome</numero>\n";
   }
   elsif($i == 5){
      print ARQ "\t\t<endereco_1>$nome</endereco_1>\n";
   }
   elsif($i == 6){
      print ARQ "\t\t<endereco_2>$nome</endereco_2>\n";
   }
   elsif($i == 7){
      print ARQ "\t\t<bairro>$nome</bairro>\n";
   }
   elsif($i == 8){
      print ARQ "\t\t<cep>$nome</cep>\n";
   }
   elsif($i == 9){
      print ARQ "\t\t<estado>$nome</estado>\n";
   }
   elsif($i == 10){
      print ARQ "\t\t<data_nasc>$nome</data_nasc>\n";
   }
   else{
   }
   $i++;
 }
 print ARQ "\t</registro>\n";
}

#----------------------------------------------------------------------------------------------------------------------------
print ARQ "</root>\n";
close (ARQ);
