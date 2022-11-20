# A Rota dos Dragões

### Henrique Augusto de Oliveira, 11065516

### Jefferson Leite Rodrigues Dantas, 11076316

---

### O que é

O projeto simula a movimentação de 4 dragões ao redor do ponto (0,0,0).
O usuário poderá modificar a escala e as velocidades de movimento e rotação dos dragões através de sliders. A opção de pause está disponível.
Também será possível movimentar a camera do usuário através do campo.
Tomou-se como base o projeto LookAt.

---

### Implementação

#### main.cpp

Aqui foi definido o ponto de entrada da aplicação, colocando o título da tela como "A Rota dos Dragões"

```
#include "window.hpp"

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    Window window;
    window.setOpenGLSettings({.samples = 4});
    window.setWindowSettings({
        .width = 800,
        .height = 800,
        .title = "A Rota dos Dragões",
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
   Abaixo estão respectivamente as variáveis que vão controlar a escala e a
   altura dos dragões.
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
```

### window.cpp

No **onEvent()** vamos capturar os eventos do teclado e mouse para movimentação da camera

```
/**
 * Aqui captura-se eventos do teclado e mouse para movimentação da camera
 */
void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_q)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_truckSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }
}
```

No **onCreate** habilitamos o teste de profundidade, criamos o VBO, EBO e VAO usando m_vertices e m_indices.
Aqui também carregamos o arquivo do dragão.obj e definimos os valores iniciais das posições, ângulos, alturas e escalas dos dragões.

```
/**
 * No onCreate habilitamos o teste de profundidade, criamos o VBO, EBO e VAO
 * usando m_vertices e m_indices. Aqui também carregamos o arquivo do dragão.obj
 * e definimos os valores iniciais das posições, ângulos, alturas e escalas dos
 * dragões.
 */
void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_program);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  // Carregando modelo do dragão
  loadModelFromFile(assetsPath + "dragon.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  /**
   *Abaixo vamos definir a escala e altura inicias dos dragões
   */
  scale = 0.1f;
  height = 0.0f;

  /**
   *Aqui define-se a posição e o ângulo inicial de cada um dos 4 dragões
   */
  dragons[0].position.x = -2.0f;
  dragons[0].position.y = height;
  dragons[0].position.z = 2.0f;
  dragons[0].angle = 180.0f;

  dragons[1].position.x = 2.0f;
  dragons[1].position.y = height;
  dragons[1].position.z = 2.0f;
  dragons[1].angle = 270.0f;

  dragons[2].position.x = 2.0f;
  dragons[2].position.y = height;
  dragons[2].position.z = -2.0f;
  dragons[2].angle = 360.0f;

  dragons[3].position.x = -2.0f;
  dragons[3].position.y = height;
  dragons[3].position.z = -2.0f;
  dragons[3].angle = 90.0f;
}
```

A função **loadModelFromFile** é responsável por carregar o modelo do dragão através do arquivo dragon.obj

```
/**
 * Função responsável por carregar o modelo do dragão através do arquivo
 * dragon.obj
 */
void Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}
```

A função **onPaint()** é chamada a cada frame para desenhar os dragões na tela.

```
/**
 *A função onPaint() é chamada a cada frame para desenhar os dragões na tela
 */
void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glBindVertexArray(m_VAO);

  /**
   * Chamamos a função drawDragon para cada dos 4 dragões passando a posição do
   * dragão no array e sua cor rgb
   */
  drawDragon(0, 1.0f, 1.0f, 1.0f);
  drawDragon(1, 0.0f, 1.0f, 1.0f);
  drawDragon(2, 1.0f, 0.0f, 1.0f);
  drawDragon(3, 1.0f, 1.0f, 0.0f);

  abcg::glBindVertexArray(0);

  // Draw ground
  m_ground.paint();

  abcg::glUseProgram(0);
}
```

Na função **drawDragon()** desenha-se os dragões de acordo com suas posições, ângulos, alturas, escalas e cores.

```
/**
* Aqui desenha-se os dragões de acordo com suas posições, ângulos, alturas, escalas e cores
*/
void Window::drawDragon(int i, float color_r, float color_g, float color_b) {
  glm::mat4 model{1.0f};
  model = glm::translate(
      model, glm::vec3(dragons[i].position.x, height, dragons[i].position.z));
  model =
      glm::rotate(model, glm::radians(dragons[i].angle), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(scale));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, color_r, color_g, color_b, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
}
```

No **onPaintUI** criamos a interface que permite ao usuário pausar o jogo, alterar as
alturas, escalas e velocidades de movimento e rotação dos dragões.

```
/**
 * Aqui criamos a interface que permite ao usuário pausar o jogo, alterar as
 *alturas, escalas e velocidades de movimento e rotação dos dragões.
 */
void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  auto const widgetSize{ImVec2(218, 180)};
  ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
  ImGui::SetNextWindowSize(widgetSize);
  ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

  {
    ImGui::RadioButton("start", &startGame, 1);
    ImGui::SameLine();
    ImGui::RadioButton("pause", &startGame, 0);
    ImGui::SliderFloat("Vel. Movimento", &MovementVelocity, 0.0f, 5.0f, "%.1f");
    ImGui::SliderFloat("Vel. Rotação", &RotationVelocity, 0.0f, 200.0f, "%.1f");
    ImGui::SliderFloat("Escala", &scale, 0.1f, 0.2f, "%.2f");
    ImGui::SliderFloat("Altura", &height, 0.0f, 1.0f, "%.2f");
  }
  ImGui::End();
}
```

A função **onUpdate()** executada a cada frame é responsável por chamar **updateDragonPosition()** que altera a posição e rotação dos dragões. Aqui também é atualizada a posição da câmera do usuário.

```
/**
 * Função executada a cada frame responsável por chamar a função
 * updateDragonPosition que altera a posição dos dragões e da câmera controlada
 * pelo usuário
 */
void Window::onUpdate() {
  updateDragonPosition(0);
  updateDragonPosition(1);
  updateDragonPosition(2);
  updateDragonPosition(3);
}

/**
 * updateDragonPosition é responsável por alterar a posição e rotação dos
 * dragões. Aqui também é atualizada a posição da câmera do usuário.
 */
void Window::updateDragonPosition(int i) {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  if (startGame) {
    /**
     * As variáveis X, Z e A são utilizadas somente para facilitar o
     * entendimento na hora de utilizar nos if's.
     */
    float X = dragons[i].position.x;
    float Z = dragons[i].position.z;
    float A = dragons[i].angle;

    // Rotacionando no Ponto A
    if (X <= -2 && Z >= 2 && A <= 180.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do Ponto A ao Ponto B
    else if (X <= 2 && Z >= 2 && A >= 180.0f) {
      X += deltaTime * MovementVelocity;
    }

    // Rotacionando no Ponto B
    if (X >= 2 && Z >= 2 && A <= 270.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do Ponto B ao Ponto C
    else if (X >= 2 && Z >= -2 && A >= 270.0f) {
      Z -= deltaTime * MovementVelocity;
    }

    // Rotacionando no Ponto C
    else if (X >= 2 && Z <= -2 && A <= 360.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do Ponto C ao Ponto D
    else if (X >= -2 && Z <= -2 && A >= 360) {
      X -= deltaTime * MovementVelocity;
    }

    // Ao chegar no Ponto D definimos o ângulo = 0, pois 360° == 0°
    else if (X <= -2 && Z <= -2 && A >= 360) {
      A = 0.0f;
    }

    // Rotacionando no Ponto D
    else if (X <= -2 && Z <= -2 && A <= 90.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do ponto D ao Ponto A
    else if (X <= -2 && Z <= 2 && A >= 90.0f) {
      Z += deltaTime * MovementVelocity;
    }

    /**
     * Atualizando posições e ângulos com os novos valores
     */
    dragons[i].position.x = X;
    dragons[i].position.z = Z;
    dragons[i].angle = A;
  }

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
}

```

A seguir uma imagem que ilustra como é a movimentação dos dragões.
![Aplicação](https://i.imgur.com/kjXos8U.png)

Os círculos representam vértices com as coordenadas x e z(y é controlada pelo usuário).
Os graus representam a rotação do dragão naquela aresta em questão.

Temos as funções **onResize()** responsável por redesenhar o objeto toda vez que a tela é redimensionada e a função **onDestroy()** que libera os recursos do OpenGL.

```
void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  m_ground.destroy();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}
```

Os arquivos **lookat.frag**, **lookat.vert**, **camera.hpp**, **camera.cpp**, **ground.hpp** e **ground.cpp** foram pegos do projeto **lookat** sem qualquer alteração relevante.

O arquivo **dragon.obj** foi encontrado no seguinte endereço: https://github.com/cgibson/OpenGL-Examples/blob/master/models/dragon.obj
