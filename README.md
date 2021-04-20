# CompDir
Compara diretórios, procurando por subdiretórios repetidos/similares para ajudar a abrir espaço

Selecione um diretório e clique em Listar. O programa mostrará uma lista de subdiretórios. Retire os subdiretórios indesejados (por serem muito grandes, por não ter acesso, etc) e clique em Analisar. O programa comparará todos os subdiretórios, recursivamente, uns com os outros. Subdiretórios com nomes iguais e mesma magnitude de tamanho (i.e. uma diferença em tamanho de até 10 vezes) serão apresentados na listagem final, ordenados do maior para o menor (desde que tenham pelo menos 1 MB, o que exclui uma grande quantidade de diretórios insignificantes, que apenas deixariam a análise lenta).

Com isso diretórios repetidos (resultado de backup mal feito, por exemplo) podem ser localizados, e medidas adicionais podem ser tomadas para excluir ou unir as réplicas, liberando espaço em disco.

Essa versão foi testada apenas no Linux (Debian Buster).
