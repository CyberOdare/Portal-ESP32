# Portal Cativo Wi-Fi

Este é um código para criação de um portal cativo Wi-Fi, que pode ser utilizado para capturar credenciais de usuários ao conectar a uma rede Wi-Fi. 
O portal cativo cria uma página de login falsa que solicita a senha do usuário para acesso à rede, enquanto captura as credenciais inseridas.

## Funcionalidades

- Cria um ponto de acesso Wi-Fi com um portal cativo.
- Captura e exibe as credenciais inseridas pelos usuários.
- Permite limpar a lista de credenciais armazenadas.
- Possibilidade de alterar o SSID da rede Wi-Fi através de uma página web.

## Requisitos

- Placa ESP32 (ou similar) com suporte a Wi-Fi.
- Bibliotecas necessárias: WiFi.h, DNSServer.h, WebServer.h.

## Configuração

Antes de utilizar o código, certifique-se de ajustar as configurações do usuário de acordo com suas necessidades:

- `NOME_REDE`: Nome da rede Wi-Fi falsa.
- `TITULO_INICIAL`: Título do portal cativo.
- `SUBTITULO`: Subtítulo exibido no portal cativo.
- `TITULO`: Título da página de login.
- `CORPO`: Mensagem exibida na página de login.
- `CORPO_ENVIO`: Mensagem exibida após o envio das credenciais.
- `TITULO_SENHA`: Título da página de exibição de credenciais.
- `TITULO_LIMPAR`: Título da página de limpeza de credenciais.
- `TITULO_SSID`: Título da página de alteração de SSID.

## Páginas Web

O portal cativo possui as seguintes páginas web para serem acessadas através de um navegador:

1. **Página de Login:** Abra qualquer página da web para ser redirecionado automaticamente para a página de login onde deverá inserir a senha falsa.
2. **Exibição de Credenciais:** Acesse `http://172.0.0.1/creds` para visualizar as credenciais capturadas.
3. **Alteração de SSID:** Acesse `http://172.0.0.1/ssid` para alterar o SSID da rede Wi-Fi.

## Uso

1. Carregue o código em sua placa ESP32.
2. Conecte-se à rede Wi-Fi falsa criada pelo dispositivo.
3. Abra um navegador e acesse qualquer página da web; você será redirecionado automaticamente para o portal cativo.
4. Insira uma senha falsa e clique em "Conectar" para enviar as credenciais.
5. As credenciais serão exibidas na página de exibição de credenciais.

## Notas

- Este código é fornecido apenas para fins educacionais e de teste. Não é recomendado o uso para captura ilegal de credenciais.
- Certifique-se de utilizar o código de forma ética e legal, respeitando a privacidade e os direitos dos usuários.
