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

  // Matriz que armazena os as posições iniciais dos vértices
  float positionsInitialValues[9][2] = {
      {-1.0f, -1.0f},  {-0.75f, 1.0f}, {-0.5f, -1.10f},
      {-0.25f, 0.75f}, {0.f, -1.20f},  {0.25f, 1.0f},
      {0.5f, -1.1f},   {0.75f, 0.75f}, {1.0f, -1.0f}};

  /** Matriz que armazena as posições dos vértices, esses valores poderão ser
   * modificados por sliders
   */
  float positionsValues[9][2]{};

  // Matriz que armazena os as cores iniciais dos vértices
  float colorsInitialValues[9][3] = {
      {0.00f, 0.00f, 0.00f}, {0.00f, 0.00f, 1.00f}, {1.00f, 0.00f, 1.00f},
      {0.00f, 1.00f, 1.00f}, {0.00f, 1.00f, 0.00f}, {1.00f, 1.00f, 0.00f},
      {1.00f, 0.00f, 0.00f}, {1.00f, 1.00f, 0.00f}, {1.00f, 1.00f, 1.00f}};

  /** Matriz que armazena as cores dos vértices, esses valores poderão ser
   * modificados por sliders
   */
  float colorsValues[9][3]{};

  /**
   * Variável que sinaliza quando há uma modificação de algum valor de posição
   * ou cor Quando change = true significa que o desenho na tela pode ser
   * atualizado
   */
  bool change{true};

  /**
   * Função que cria os recursos identificados por m_VAO, m_VBOPositions e
   * m_VBOColors. A função é chamada sempre que o desenho vai ser atualizado.
   * Passamos a quantidade de lados do objeto como parâmetro da função.
   */
  void setupModel(int sides);

  /**
   * Função que irá setar as posições e cores dos vértices com os valores
   * iniciais
   */
  void setValues();
};

#endif