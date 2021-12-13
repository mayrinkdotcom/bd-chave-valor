# bd-chave-valor

![C logo image](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![CMake logo image](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![GitHub Actions logo image](https://img.shields.io/badge/GitHub_Actions-2088FF?style=for-the-badge&logo=github-actions&logoColor=white)


<hr/>

Banco de Dados Chave-Valor utilizando threads de processamento. :)

### Projeto

Este repositório contém os códigos e implementações utilizados para a construção de um Banco de Dados Chave-Valor, para a obtenção de pontos constituintes da disciplina de Sistemas Operacionais na [PUC Minas](https://pucminas.br), ministrada pelo professor [Pedro Penna](https://github.com/ppenna/). O escopo do projeto, bem como suas orientações e seu processo avaliativo, estão descritos [neste arquivo](./docs/bd-chave-valor.pdf).

O projeto se baseia em programação assíncrona para realizar os processos internos. Para interagir com a CLI, foi criada uma thread especializada para essa função. No momento da escrita do arquivo de log, foi desenvolvida outra thread, que não depende da thread anterior, tornando o processo mais otimizado.

### Como usar

  1. Faça o clone do repositório, seguindo os comandos abaixo

```bash
git clone https://github.com/mayrinkdotcom/bd-chave-valor.git
cd bd-chave-valor
```

  2. Na pasta do projeto, digite o comando `make` para realizar a compilação do projeto localmente.
