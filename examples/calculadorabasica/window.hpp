#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaintUI() override;

private:
  // Fonte que aumenta o tamanho do texto
  ImFont *m_font{};

  // Armazena o resultado da conta
  std::string result{"0.000"};

  /*
   * Texto que mostra se a conta está sendo feita utilizando os valores dos
   * sliders ou dos inputs
   */
  std::string option{"A conta está utilizando os valores dos Sliders"};

  /*
   * Booleano que indica se estamos utilizando o slider ou o input na operação,
   * se sliderChoosed for true, está se utilizando os sliders, se for false,
   * está se utilizando os inputs
   */
  bool sliderChoosed{true};

  // float que armazena os valores dos sliders
  float slider01{50};
  float slider02{50};

  // double que armazena os valores dos inputs
  double input01{0.0};
  double input02{0.0};

  /*
   * Aqui define-se o slider que irá conter as operações
   */
  enum Element {
    Element_Sum,
    Element_Subtraction,
    Element_Multiplication,
    Element_Division,
    Element_COUNT
  };
  const char *elems_names[Element_COUNT] = {"+", "-", "X", "÷"};
  int elem = Element_Sum;

  /* Função que faz o calculo recebendo dois valores, ela também é responsável
   * por atualizar o texto do resultado mostrado ao usuário
   */
  void calculate(double n1, double n2);

  // Função para adicionar espaços entre as ferramentas
  void doSpacing(int spaces);
};

#endif
