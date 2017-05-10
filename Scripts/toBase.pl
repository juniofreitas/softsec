# Transforma para XML

use Time::localtime;

# verifica se o parametro foi passado corretamente
if($#ARGV+1!=2){
   print "Parametros errados!!!\nO certo eh \"toBase.pl [ARQ][DIR.DESTINO]\"";
   exit;
}
else{
   # recebe argumento
   $arq = $ARGV[0];
   $res = $ARGV[1];
}
print "<<CRIA ARQUIVO NOMES DO ARQUIVO DE REGISTRO PASSADO>>\n";

# abre o arquivo passado
open(XML, $arq) || die "O arquivo $arq nao pode ser aberto";
#data atual
$data = sprintf("%02d/%02d/%04d",localtime->mday(), localtime->mon(), localtime->year()+1900);
# cria o nome do arquivo de saida
$arquivox = sprintf("%s%02d%02d%02d%s","NOME",localtime->hour(), localtime->min(), localtime->sec(),".txt");
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
#----------------------------------------------------------------------------------------------------------------------------
$i =1;
foreach $linha(@texto){
 print "\r $i";
 $res = ($linha =~/;/);
 $resto = $';
 $res = ($resto =~/;/);
 $nome = $`;
 $i++;
 print ARQ "$nome\n";
}

#----------------------------------------------------------------------------------------------------------------------------
close (ARQ);