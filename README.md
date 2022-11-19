# Modificador de Objeto

### Jefferson Leite Rodrigues Dantas, 11076316

---

### O que é

O projeto fornece uma aplicação que permite modificar a forma de um objeto.
O usuário pode selecionar a quantidade de vértices do objeto(entre 3 a 9), mudar a posição de cada um desses vértices(em x e y) e do objeto como um todo, modificar as cores de cada um desses vértices e por fim alterar a escala e a rotação do objeto.

---

### Implementação

#### main.cpp

Aqui foi definido o ponto de entrada da aplicação, colocando o título da tela como "Modificador de Objeto"

```
#include "window.hpp"

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    Window window;
    window.setOpenGLSettings({.samples = 2, .doubleBuffering = true});
    window.setWindowSettings({
        .width = 600,
        .height = 600,
        .title = "Modificador de Objeto",
    });

    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}

```

### window.hpp

Aqui definimos a classe **Window**, responsável pelo gerenciamento da aplicação.
Armazena-se também as variáveis e métodos utilizados no window.cpp.

No código há comentários explicando a função de cada uma delas.

```
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <glm/fwd.hpp>
#include <random>

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  GLuint m_program{};

  // Quantidade de lados/vértices do objeto
  int sides{3};

  // Rotação do objeto
  float rotation{0};

  // Escala do objeto
  float scale{0.25};

  // Matriz que armazena as posições iniciais dos vértices
  float positionsInitialValues[9][2] = {
      {-1.0f, -1.0f},  {-0.75f, 1.0f}, {-0.5f, -1.10f},
      {-0.25f, 0.75f}, {0.f, -1.20f},  {0.25f, 1.0f},
      {0.5f, -1.1f},   {0.75f, 0.75f}, {1.0f, -1.0f}};

  /** Matriz que armazena as posições dos vértices, esses valores poderão ser
   * modificados pelos sliders
   */
  float positionsValues[9][2]{};

  // Matriz que armazena as cores iniciais dos vértices
  float colorsInitialValues[9][3] = {
      {0.00f, 0.00f, 0.00f}, {0.00f, 0.00f, 1.00f}, {1.00f, 0.00f, 1.00f},
      {0.00f, 1.00f, 1.00f}, {0.00f, 1.00f, 0.00f}, {1.00f, 1.00f, 0.00f},
      {1.00f, 0.00f, 0.00f}, {1.00f, 1.00f, 0.00f}, {1.00f, 1.00f, 1.00f}};

  /** Matriz que armazena as cores dos vértices, esses valores poderão ser
   * modificados pelos sliders
   */
  float colorsValues[9][3]{};

  /**
   * Variável que sinaliza quando há modificação de algum valor de posição
   * ou cor. Quando change = true significa que o desenho na tela pode ser
   * atualizado
   */
  bool change{true};

  /**
   * Função que cria os recursos identificados por m_VAO, m_VBOPositions e
   * m_VBOColors. A função é chamada sempre que o desenho vai ser atualizado.
   * Passamos a quantidade de lados/vértices do objeto como parâmetro da função.
   */
  void setupModel(int sides);

  /**
   * Função que irá setar as posições e cores dos vértices com os valores
   * iniciais
   */
  void setValues();
};

#endif
```

### window.cpp

No **onCreate()** define-se o vertexShader e o fragmentShader, após isso já desenha-se o objeto na tela com os valores iniciais definidos pelo programador chamando a função **setValues()**.

```
void Window::onCreate() {
  /**
   * Definição do vertexShader
   */
  auto const *vertexShader{R"gl(#version 300 es

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    uniform vec2 translation;
    uniform float scale;
    uniform float rotation;

    out vec4 fragColor;

    void main() {
      float sinAngle = sin(rotation);
      float cosAngle = cos(rotation);
      vec2 rotated = vec2(inPosition.x * cosAngle - inPosition.y * sinAngle,
                    inPosition.x * sinAngle + inPosition.y * cosAngle);

      vec2 newPosition = rotated * scale + translation;
      gl_Position = vec4(newPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  /**
   * Definição do fragmentShader
   */
  auto const *fragmentShader{R"gl(#version 300 es

    precision mediump float;

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Setando valores iniciais
  setValues();
}
```

A função **onPaint()** é chamada a cada frame, porém ela só irá atualizar o objeto na tela quando o usuário realizar alguma alteração usando a interface, que define a variável **change** como true.

```
void Window::onPaint() {
  /**
   * O desenho só será modificado quando o usuário fizer alguma alteração
   * utilizando a interface ImGUI, colocando change para true
   */
  if (change) {
    abcg::glClear(GL_COLOR_BUFFER_BIT);

    /*
     * Chama função setupModel responsável por criar os recursos identificados
     * por m_VAO, m_VBOPositions e m_VBOColors
     */
    setupModel(sides);

    // Definindo a ViewPort
    abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

    // Ativa os shaders compilados no programa m_program
    abcg::glUseProgram(m_program);

    // Definindo a escala do objeto
    auto const scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
    abcg::glUniform1f(scaleLocation, scale);

    // Definindo a rotação do objeto
    auto const rotationLocation{
        abcg::glGetUniformLocation(m_program, "rotation")};
    abcg::glUniform1f(rotationLocation, rotation);

    // Renderização do objeto na tela utilizando GL_TRIANGLE_STRIP
    abcg::glBindVertexArray(m_VAO);
    abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, sides);
    abcg::glBindVertexArray(0);

    abcg::glUseProgram(0);

    change = false;
  }
}
```

A função **onPaintUI()** pode ser dividida em 4 partes, cada uma delas possui alguma função especifica. No código há comentários explicando o que foi feito em cada parte.

Primeiramente temos a parte da interface que está na parte superior esquerda da aplicação. Nesta divisão o usuário pode selecionar a quantidade de vértices do objeto, alterar a posição de cada um dos vértices em x e y e ainda resetar todas as modificações feitas no objeto até então, apenas a quantidade de vértices não é resetada, pois imagina-se que o usuário vá querer apenas apagar alguma alteração brusca feita pelos sliders.

```
void Window::onPaintUI() {

  {
    /**
     * Nas linhas abaixo estou definindo o tamanho e a posição da tela onde vão
     * ficar as ferramentas que alteram a quantidade de lados e a
     * posição dos vértices
     */
    auto const widgetPositionSize{ImVec2(250, 150)};
    ImGui::SetNextWindowPos(ImVec2(5, 5));
    ImGui::SetNextWindowSize(widgetPositionSize);
    auto const windowFlags{ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin("Position", nullptr, windowFlags);

    /**
     * Botão que reseta o desenho na tela(posição, escala, cores e rotação), mantendo a
     * quantidade de lados atual do objeto
     */
    if (ImGui::Button("Resetar Desenho", ImVec2(-1, 30))) {
      setValues();
      change = true;
    }

    /**
     * Sliders que alteram a quantidade de lados e a posição dos
     * vértices do objeto
     */
    if (ImGui::SliderInt("Sides", &sides, 3, 9) ||
        ImGui::SliderFloat2("Lado 01", positionsValues[0], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 02", positionsValues[1], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 03", positionsValues[2], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 04", positionsValues[3], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 05", positionsValues[4], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 06", positionsValues[5], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 07", positionsValues[6], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 08", positionsValues[7], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("Lado 09", positionsValues[8], -5.00f, 5.00f,
                            "%.2f")) {
      change = true;
    }

    ImGui::End();
  }
```

Na divisão do canto superior direito temos a opção de modificar as cores dos vértices utilizando o padrão RGB.

```
  {
    /**
     * Nas linhas abaixo estou definindo o tamanho e a posição da tela onde vão
     * ficar as ferramentas que alteram as cores dos vértices
     */
    auto const widgetColorSize{ImVec2(320, 150)};
    ImGui::SetNextWindowPos(
        ImVec2(m_viewportSize.x - widgetColorSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetColorSize);
    auto const windowFlags2{ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin("Color", nullptr, windowFlags2);

    auto colorEditFlags{ImGuiColorEditFlags_NoTooltip |
                        ImGuiColorEditFlags_NoPicker};

    /**
     * Ferramentas que alteram as cores dos vértices
     */
    if (ImGui::ColorEdit3("Vértice 01", colorsValues[0], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 02", colorsValues[1], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 03", colorsValues[2], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 04", colorsValues[3], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 05", colorsValues[4], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 06", colorsValues[5], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 07", colorsValues[6], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 08", colorsValues[7], colorEditFlags) ||
        ImGui::ColorEdit3("Vértice 09", colorsValues[8], colorEditFlags)) {
      change = true;
    }
    ImGui::End();
  }
```

No segmento inferior esquerdo dispomos de 4 botões que alteram em 0.1 a posição do objeto como um todo para direita, esquerda, cima e baixo.

```
{

    /**
     * Nas linhas abaixo estou definindo o tamanho e a posição da tela onde vão
     * ficar os botões que movimentam o objeto inteiro
     */
    auto const widgetMovimentationSize{ImVec2(100, 125)};
    ImGui::SetNextWindowPos(
        ImVec2(5, m_viewportSize.y - widgetMovimentationSize.y - 5));
    ImGui::SetNextWindowSize(widgetMovimentationSize);
    auto const windowFlags2{ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin("Movimentation", nullptr, windowFlags2);

    /**
     *Basicamente abaixo temos 4 botões, cada um deles vais ser responsável por
     *movimentar o objeto em 0.1 de distância para esquerda, direita, cima e
     *baixo
     */
    if (ImGui::Button("Esquerda")) {
      for (int i = 0; i < sides; i++) {
        positionsValues[i][0] = positionsValues[i][0] - 0.1;
        change = true;
      }
    }

    if (ImGui::Button("Direita")) {
      for (int i = 0; i < sides; i++) {
        positionsValues[i][0] = positionsValues[i][0] + 0.1;
        change = true;
      }
    }

    if (ImGui::Button("Cima")) {
      for (int i = 0; i < sides; i++) {
        positionsValues[i][1] = positionsValues[i][1] + 0.1;
        change = true;
      }
    }

    if (ImGui::Button("Baixo")) {
      for (int i = 0; i < sides; i++) {
        positionsValues[i][1] = positionsValues[i][1] - 0.1;
        change = true;
      }
    }

    ImGui::End();
  }
```

Por fim, na seção do canto inferior direito ficam os sliders que rotacionam e alteram a escala do objeto como um todo.

```
  {
    /**
     * Nas linhas abaixo estou definindo o tamanho e a posição da tela onde vão
     * ficar o Slider que rotacionam e mudam a escala do objeto inteiro
     */
    auto const widgetRotationSize{ImVec2(250, 70)};
    ImGui::SetNextWindowPos(
        ImVec2(m_viewportSize.x - widgetRotationSize.x - 5,
               m_viewportSize.y - widgetRotationSize.y - 5));
    ImGui::SetNextWindowSize(widgetRotationSize);

    auto const windowFlags2{ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin("Rotation", nullptr, windowFlags2);

    /**
     * Sliders que alteram a escala e a rotação do objeto
     */
    if (ImGui::SliderFloat("Scale", &scale, 0.01f, 1.0f, "%.2f") ||
        ImGui::SliderFloat("Rotation", &rotation, -10.00f, 10.0f, "%.2f")) {
      change = true;
    }

    ImGui::End();
  }
```

A seguir uma imagem que ilustra melhor a aplicação
![Aplicação](https://i.imgur.com/E3VACAx.png)

Temos as funções **onResize()** responsável por redesenhar o objeto toda vez que a tela é redimensionada e a função **onDestroy()** que libera os recursos do OpenGL.

```
/**
 *Função chamada ao redimensionar a tela
 */
void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  change = true;
}

/**
 *Função usada para liberar os recursos do OpenGL
 */
void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}
```

A função **setupModel** cria o VAO e os VBOs de posições e cores, vinculando eles aos shaders.
Aqui também é feito o armazenamento das cores e das posições dos vértices no arranjo que é responsável por colocar a informação nos respectivos VBOs.

```
/**
 * Função que cria os recursos identificados por m_VAO, m_VBOPositions e
 * m_VBOColors. A função é chamada sempre que o desenho vai ser atualizado.
 * Passamos a quantidade de lados do objeto como parâmetro.
 */
void Window::setupModel(int sides) {
  // As três linhas abaixo liberam recursos anteriores
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  /*
   * Abaixo estão os arranjos que armazenam as posições e cores dos vértices
   */
  std::vector<glm::vec2> positions;
  std::vector<glm::vec3> colors;

  /**
   * Essa iteração é responsável por armazenar as posições e cores dos vértices
   * dentro do arranjo positions e colors
   */
  for (int i = 0; i < sides; i++) {
    positions.emplace_back(
        glm::vec2{positionsValues[i][0], positionsValues[i][1]});
    colors.push_back(
        glm::vec3{colorsValues[i][0], colorsValues[i][1], colorsValues[i][2]});
  }

  // Gerando VBO de posições
  abcg::glGenBuffers(1, &m_VBOPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Gerando VBO de cores
  abcg::glGenBuffers(1, &m_VBOColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Capturando a localização dos atributos dentro de program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Criando VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Vinculando atributos do vértice ao VAO
  abcg::glBindVertexArray(m_VAO);

  /*
  Abaixo está a especificação dos VBOs de posição e cores com o vertex shader
  */
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Encerrando o vinculo atributos do vértice ao VAO
  abcg::glBindVertexArray(0);
}
```

Por fim a função **setValues()** define as posições e cores dos vértices passando os valores iniciais definidos pelo programador.

```
/**
 * Função que irá setar as posições e cores dos vértices com os valores iniciais
 */
void Window::setValues() {
  scale = 0.25;
  rotation = 0;
  for (int i = 0; i < 9; i++) {
    positionsValues[i][0] = positionsInitialValues[i][0];
    positionsValues[i][1] = positionsInitialValues[i][1];
    colorsValues[i][0] = colorsInitialValues[i][0];
    colorsValues[i][1] = colorsInitialValues[i][1];
    colorsValues[i][2] = colorsInitialValues[i][2];
  }
}
```
