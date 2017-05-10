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
print "<<CRIA ARQUIVO CORRIGIDO DO ARQUIVO DE PARES PASSADO>>\n";

# abre o arquivo passado
open(REG, $arq) || die "O arquivo $arq nao pode ser aberto";
#data atual
$data = sprintf("%02d/%02d/%04d",localtime->mday(), localtime->mon(), localtime->year()+1900);
# cria o nome do arquivo de saida
$arquivox = sprintf("%s%02d%02d%02d%s","$arq",localtime->hour(), localtime->min(), localtime->sec(),".txt");
# cria o arquivo de saida
open  (ARQ, ">$res/$arquivox") || die "Erro de criacao de arquivo";
# ...
print "Inicializando ...\n";

@texto = <REG>;
close (REG);

print "OK!\n";
# para cada registro do arquivo
#----------------------------------------------------------------------------------------------------------------------------
$i =1;
foreach $linha(@texto){
 print "\r $i";
 ($id1,$nome1,$id2,$nome2) = split(/\t/,$linha);
 chomp($nome2);
 $reg1 = "bdb"."\t".$id1."\t".$nome1;
 $reg2 = "bdb"."\t".$id1."\t".$nome2;
 #$res = ($linha =~/;/);
 #$resto = $';
 #$res = ($resto =~/;/);
 #$nome = $`;
 #$i++;
 print ARQ "$reg1\n$reg2\n";
}
#----------------------------------------------------------------------------------------------------------------------------
close (ARQ);