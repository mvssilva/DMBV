### Descrição do Programa

Este programa foi desenvolvido para estudar a relação entre a **densidade de um grafo** e a **quantidade mínima de d-branchs** necessárias para formar uma árvore. O objetivo principal é analisar como diferentes valores do parâmetro **alfa** se aproximam dos resultados ótimos obtidos para grafos de uma instância específica.

#### Funcionamento do Programa:
1. **Leitura dos Dados**:
   - O programa lê um arquivo que contém os **resultados ótimos** para grafos de uma instância padrão (std). Esses resultados representam a quantidade mínima de d-branchs necessárias para formar uma árvore em cada grafo.

2. **Armazenamento dos Resultados Ótimos**:
   - Os valores ótimos são armazenados em uma estrutura de dados para posterior comparação.

3. **Busca pelo Melhor Valor de Alfa**:
   - O programa realiza uma busca para identificar qual valor do parâmetro **alfa** mais se aproxima dos resultados ótimos. Essa busca é feita comparando os resultados ótimos com os valores obtidos para diferentes valores de alfa.

4. **Salvamento do Valor de Alfa**:
   - O valor de alfa que melhor se aproxima do resultado ótimo é salvo para cada grafo analisado. Isso permite uma análise posterior da relação entre a densidade do grafo e o valor de alfa ideal.

#### Objetivo:
- O estudo visa entender como a densidade do grafo influencia a escolha do valor de alfa para a formação de árvores com o mínimo de d-branchs. Isso pode ser útil para otimizar algoritmos que envolvem a construção de árvores em grafos, especialmente em contextos de otimização e análise de redes.

#### Saída Esperada:
- O programa gera um arquivo contendo os valores de alfa que melhor se ajustam aos resultados ótimos para cada grafo analisado. Essa informação pode ser usada para inferir padrões ou regras sobre a escolha de alfa em função da densidade do grafo.
