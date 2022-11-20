#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
#include "ground.hpp"

struct Vertex {
  glm::vec3 position;

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  // Definindo camera e variáveis que a controlam
  Camera m_camera;
  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};

  // A struct do Dragão vai possuir o vetor posição e o ângulo dele
  struct Dragon {
    glm::vec3 position{};
    float angle{};
  };

  /*
   *Abaixo estão respectivamente as variáveis que vão controlar a escala e a
   *altura dos dragões.
   */
  float scale{};
  float height{};

  // Criando o array de dragões
  std::array<Dragon, 4> dragons;

  /*
   *Variável pause, se startGame for 0 o jogo está pausado, se startGame for 1
   *o jogo está rodando
   */
  int startGame{0};

  /* As variáveis abaixo alteram respectivamente a velocidade de movimento e
   */
  float MovementVelocity{0.5f};
  float RotationVelocity{50.0f};

  // Objeto que desenha o chão
  Ground m_ground;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void drawDragon(int i, float color_r, float color_g, float color_b);
  void updateDragonPosition(int i);
};

#endif