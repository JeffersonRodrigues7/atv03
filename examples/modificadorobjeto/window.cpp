#include "window.hpp"
#include "imgui.h"

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
   * Definição do fragmetnShader
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
     * Botão que reseta o desenho na tela(posição, escala, cores e rotação),
     * mantendo a quantidade de lados atual do objeto
     */
    if (ImGui::Button("Resetar Desenho", ImVec2(-1, 30))) {
      setValues();
      change = true;
    }

    /**
     * Sliders que alteram a quantidade de lados e a posição dos
     * vértices do objeto
     */
    if (ImGui::SliderInt("Qtd V.", &sides, 3, 9) ||
        ImGui::SliderFloat2("V 01", positionsValues[0], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 02", positionsValues[1], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 03", positionsValues[2], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 04", positionsValues[3], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 05", positionsValues[4], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 06", positionsValues[5], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 07", positionsValues[6], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 08", positionsValues[7], -5.00f, 5.00f,
                            "%.2f") ||
        ImGui::SliderFloat2("V 09", positionsValues[8], -5.00f, 5.00f,
                            "%.2f")) {
      change = true;
    }

    ImGui::End();
  }
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
    if (ImGui::SliderFloat("Escala", &scale, 0.01f, 1.0f, "%.2f") ||
        ImGui::SliderFloat("Rotação", &rotation, -10.00f, 10.0f, "%.2f")) {
      change = true;
    }

    ImGui::End();
  }
}

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
