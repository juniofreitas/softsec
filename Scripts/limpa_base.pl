#!c:\perl\bin\perl.exe

###############################################################################################
#  Script limpa_base.pl                                                                       #
#  Esse script limpa uma base de documentos                                                   #
###############################################################################################

# #!/usr/bin/perl
use Time::localtime;

# verifica se o parametro foi passado corretamente
if($#ARGV+1!=1){
   print "Parametros errados!!!\nO certo eh \"limpa_base.pl [ARQUIVO.txt]\"";
   exit;
}
else{
   # recebe argumento
   $arq = $ARGV[0];
}

print "LIMPA LISTA DE DOCUMENTOS\n";
#data atual
#$data = sprintf("%02d/%02d/%04d",localtime->mday(), localtime->mon(), localtime->year()+1900);
# abre o arquivo passado
open(XML, $arq) || die "O arquivo $arq nao pode ser aberto";
# cria o nome do arquivo de saida
$arquivox = sprintf("%s%s","L".$arq,".txt");
# cria o arquivo de saida
open  (ARQ, ">$arquivox");
#
print "Inicializando...\n";
@texto = <XML>;
#chomp (@texto); #Retira quebra de linhas
close (XML);
#$texto = "@texto";
#$texto =~ s/\t|\r|\n|\f//g;

print "OK!\n";

# para cada registro do arquivo
for($i=0; $i<$#texto+1 ;$i++ ){
   $linha1 = @texto[$i];
   $prefixo = trim($linha1);
   $prefixo =~ tr/A-Z/a-z/;
   $prefixo =~ s/\t/ /g;
   $prefixo =~ s/[^\w àáâãäåæçèéêëìíîïðñòóôõöøùúûüý]//g;
   #$prefixo =~ s/[àáâãäåæçèéêëìíîïðñòóôõöøùúûüý]//g;
   $prefixo =~ s/\s*$//;
   #$prefixo =~ s/(\d+)//g;
   #@lista = split(/\s+/, $prefixo);
   #$id1 = $lista[0];
   #$id2 = $lista[1];
   #if($id1 ne ""){
   #   $prefixo = $id1."\t".$id2."\t"."@lista[2..$#lista]";
   #}
   #@novo = '';
   #for($j=0;$j<$#lista+1;$j++){
   #   if(length($lista[$j])>2){
   #      @novo[$#novo+1] = $lista[$j];
   #   }
   #}
   #$prefixo = trim(join(' ', @novo));

   #------

   if($prefixo eq ""){
      print "VAZIO\n";
      #print ARQ "VAZIO\n";
   }
   else{
      print "\r$i";
      print ARQ "$prefixo\n";
   }
}

close(ARQ);

#-----------------------------------------------------------------------------------------------------------------------
sub trim {
        return unless defined wantarray;  # void context, do nothing
        my @parms = @_;
        for (@parms) { s/(^\s*)|(\s*$)//g }
        return wantarray ? @parms : $parms[0];
}