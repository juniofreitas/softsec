# GERA BASE PARA A TRANSFORMAÇÃO

use Time::localtime;

# verifica se o parametro foi passado corretamente
if($#ARGV+1!=2){
   print "Parametros errados!!!\nO certo eh \"gerabase.pl [ARQ][DIR.DESTINO]\"";
   exit;
}
else{
   # recebe argumento
   $arq = $ARGV[0];
   $res = $ARGV[1];
}
print "<<CRIA BASE DE NOMES PARA COMPARACAO>>\n";

# abre o arquivo passado
open(XML, $arq) || die "O arquivo $arq nao pode ser aberto";
#data atual
$data = sprintf("%02d/%02d/%04d",localtime->mday(), localtime->mon(), localtime->year()+1900);
# cria o nome do arquivo de saida
$arquivox = sprintf("%s%02d%02d%02d%s","BASE",localtime->hour(), localtime->min(), localtime->sec(),".txt");
# cria o arquivo de saida
open  (ARQ, ">$res/$arquivox");
#
print "Inicializando ...\n";

@texto = <XML>;
chomp (@texto); #Retira quebra de linhas
close (XML);
$texto = "@texto";
$texto =~ s/\t|\r|\n|\f//g;

print "OK!\n";

#----------------------------------------------------------------------------------------------------------------------------

$i = 1;
while( $texto =~ /(<registro>)/g){
 $inicreg = $';
 $inicreg =~ /<\/registro>/;
 $fimreg = $`;

 # ID
 $fimreg =~ /<id>/;
 $' =~ /<\/id>/;
 $id = trim($`);
 # NOME
 $fimreg =~ /<nome>/;
 $' =~ /<\/nome>/;
 $nome = trim($`);
 # SOBRE NOME
 $fimreg =~ /<sobre_nome>/;
 $' =~ /<\/sobre_nome>/;
 $sobre = trim($`);
 #
 $nome = $nome . " " . $sobre;
 print "$i\t$id\t$nome\n";
 print ARQ "$i;$id;$nome;\n";
 $i++;
}
close(ARQ);

#############################################################################################################################
sub trim {
        return unless defined wantarray;  # void context, do nothing
        my @parms = @_;
        for (@parms) { s/(^\s*)|(\s*$)//g }
        return wantarray ? @parms : $parms[0];
}














