#include "window.hpp"

void Window::onCreate() {
  // Carregando a fonte que aumenta o tamanho da letra
  auto const filename{abcg::Application::getAssetsPath() +
                      "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 72.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError{"Cannot load font file"};
  }

  // O resultado inicial é a partir dos valores dos sliders
  calculate(slider01, slider02);
}

void Window::onPaintUI() {
  // Pegando tamanho da janela de aplicação
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  // "Calculadora Básica" Window
  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    auto const flags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse};
    ImGui::Begin("Calculadora Básica", nullptr, flags);
    doSpacing(1);

    // Texto que mostra o resultado da conta
    {
      ImGui::PushFont(m_font);

      // Centralizar texto
      ImGui::SetCursorPosX(
          (appWindowWidth - ImGui::CalcTextSize(result.c_str()).x) / 2);
      ImGui::Text("%s", result.c_str());
      ImGui::PopFont();

      /*
       *Texto que mostra se o resultado da conta está utilizando os valores dos
       *sliders ou dos inputs
       */
      if (sliderChoosed) {
        option = "A conta está utilizando os valores dos Sliders";
      } else {
        option = "A conta está utilizando os valores dos Inputs";
      }
      ImGui::SetCursorPosX(
          (appWindowWidth - ImGui::CalcTextSize(option.c_str()).x) / 2);
      ImGui::Text("%s", option.c_str());

      doSpacing(1);
    }

    doSpacing(2);

    // Tabela onde ficam as ferramentas da calculadora
    if (ImGui::BeginTable("Calculator Input Options", 2, 1)) {
      ImGui::TableNextRow();

      // Coluna dos Sliders
      {
        ImGui::TableSetColumnIndex(0);

        ImGui::Indent(50.f);

        // Primeiro Slider
        ImGui::Text("Primeiro número");
        if (ImGui::SliderFloat("##01", &slider01, 0.0f, 100.0f, "%.3f")) {
          sliderChoosed = true;
          calculate(slider01, slider02);
        }

        doSpacing(3);

        // Segundo Slider
        ImGui::Text("Segundo número");
        if (ImGui::SliderFloat("##02", &slider02, 0.0f, 100.0f, "%.3f")) {
          sliderChoosed = true;
          calculate(slider01, slider02);
        }

        doSpacing(3);

        // Botão dos Sliders
        ImGui::Indent(50.f);
        if (ImGui::Button("Calcular##01")) {
          sliderChoosed = true;
          calculate(slider01, slider02);
        }
      }

      // Coluna dos inputs
      {
        ImGui::TableSetColumnIndex(1);

        ImGui::Indent(50.f);

        // Primeiro Input
        ImGui::Text("Primeiro número");
        if (ImGui::InputDouble("##03", &input01, 0, 0, "%.3f")) {
          sliderChoosed = false;
          calculate(input01, input02);
        }

        doSpacing(3);

        // Segundo Input
        ImGui::Text("Segundo número");
        if (ImGui::InputDouble("##04", &input02, 0, 0, "%.3f")) {
          sliderChoosed = false;
          calculate(input01, input02);
        }

        doSpacing(3);

        // Botão dos Inputs
        ImGui::Indent(50.f);
        if (ImGui::Button("Calcular##02")) {
          sliderChoosed = false;
          calculate(input01, input02);
        }
      }

      ImGui::Unindent(50.f);
      doSpacing(1);
      ImGui::EndTable();
    }

    doSpacing(6);

    ImGui::PushFont(m_font);
    ImGui::Text("Operação");
    ImGui::Unindent(50.f);

    // Slider para escolha de operações
    const char *elem_name =
        (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
    if (ImGui::SliderInt("##05", &elem, 0, Element_COUNT - 1, elem_name)) {
      if (sliderChoosed) {
        calculate(slider01, slider02);
      } else {
        calculate(input01, input02);
      }
    }

    ImGui::PopFont();

    ImGui::End();
  }
}

/**
 *Função que faz o cálculo através de 2 números passados, faz o cast para string
 *e armazena o valor na varíavel que mostra o resultado para o usuário
 */
void Window::calculate(double n1, double n2) {
  switch (elem) {
  case 0:
    result = std::to_string(n1 + n2);
    break;
  case 1:
    result = std::to_string(n1 - n2);
    break;
  case 2:
    result = std::to_string(n1 * n2);
    break;
  case 3:
    result = std::to_string(n1 / n2);
    break;
  }
}

/*
 *Adiciona uma quantidade de espaços entre as ferramentas de acordo com um
 *valor passado5
 */
void Window::doSpacing(int spaces) {
  for (auto const i : iter::range(spaces)) {
    ImGui::Spacing();
  }
}
