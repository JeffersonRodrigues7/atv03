#include "window.hpp"

#include <cmath>
#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "box.obj");
  m_model.setupVAO(m_program);

  // Camera at (0,0,0) and looking towards the negative z
  glm::vec3 const eye{0.0f, 10.0f, 20.0f};
  glm::vec3 const at{0.0f, -5.0f, -1.0f};
  glm::vec3 const up{0.0f, 1.0f, 0.0f};
  m_viewMatrix = glm::lookAt(eye, at, up);

  planet.m_position = glm::vec3(0, 0, -4);
  planet.m_rotationAxis = glm::sphericalRand(1.0f);

  raio = 4;
  pos = 0;
  hip = sqrt(raio * raio + raio * raio);
}

void Window::onUpdate() {
  // Increase angle by 90 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // auto const Ry{glm::rotate(glm::vec4{1.0}, -m_angle * deltaTime,
  // glm::vec3(0.0, 1.0, 0.0))};

  pos += deltaTime * 1;

  // Se ponto C, então o X agora vai para a esquerda
  if (pos >= hip) {
    planet.m_position.x += -1 * deltaTime * cos(45);
    planet.m_position.z += directionZ * deltaTime * sin(45);
  } else if (pos >= (hip / 2.0)) { // Se ponto B
    planet.m_position.x += directionX * deltaTime * cos(45);
    planet.m_position.z += directionZ * deltaTime * sin(45);
  } else { // Ponto A, Inicio
    planet.m_position.x += directionX * deltaTime * sin(45);
    planet.m_position.z += directionZ * deltaTime * cos(45);
  }

  /**
    if (planet.m_position.z >= 20) {
      directionZ = -1;
    } else if (planet.m_position.z <= -(20)) {
      directionZ = 1;
    } else if (planet.m_position.z >= 0.0f) {
      directionX = -1;
    } else if (planet.m_position.z <= 0.0f) {
      directionX = 1;
    }*/
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White

  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, planet.m_position);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
  modelMatrix = glm::rotate(modelMatrix, m_angle, planet.m_rotationAxis);

  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  m_model.render();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const widgetSize{ImVec2(218, 90)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    {
      ImGui::Text("%s", std::to_string(pos).c_str());
      ImGui::Text("%s", std::to_string(planet.m_position.x).c_str());
      ImGui::Text("%s", std::to_string(planet.m_position.z).c_str());
      ImGui::PushItemWidth(120);
      static std::size_t currentIndex{};
      std::vector<std::string> const comboItems{"Perspective", "Orthographic"};

      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      ImGui::PushItemWidth(170);
      auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                        gsl::narrow<float>(m_viewportSize.y)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

        ImGui::SliderFloat("FOV", &m_FOV, 5.0f, 179.0f, "%.0f degrees");
      } else {
        m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
                                  20.0f, 0.01f, 100.0f);
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}