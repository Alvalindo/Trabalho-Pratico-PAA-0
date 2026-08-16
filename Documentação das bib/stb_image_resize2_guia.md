# Guia de Uso da stb_image_resize2

## O que é

`stb_image_resize2.h` é a biblioteca "irmã" da `stb_image.h`, também de domínio público e de arquivo único. Enquanto a `stb_image` **carrega/decodifica** imagens, essa aqui serve para **redimensionar** (mudar a resolução) de imagens que já estão descompactadas na memória — ou seja, ela funciona muito bem em conjunto com a `stb_image`.

Características principais:

- Suporta **apenas escala e translação** (redimensionar e mover a imagem dentro de uma sub-região). Não faz rotação nem distorção (shear).
- Tem versões otimizadas com SIMD: **SSE2, AVX, NEON e WASM**.
- Pode ser usada em **múltiplas threads**, dividindo o trabalho.
- Trabalha com pixels em **8 bits (uint8), 16 bits (uint16), float e half-float**.
- Tem suporte embutido para conversão de espaço de cor **sRGB**.
- Trata corretamente o **canal alfa** (transparência) durante o redimensionamento, evitando "vazamento de cor" nas bordas transparentes.

---

## 1. Instalação (mesma lógica da stb_image)

### Em UM arquivo .c/.cpp:

```c
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
```

### Nos demais arquivos que só usam a biblioteca:

```c
#include "stb_image_resize2.h"
```

> ⚠️ Mesma regra da `stb_image`: o `#define` só pode aparecer em **um único** arquivo `.c`, senão dá erro de linkagem duplicada.

---

## 2. As três "camadas" da API

A biblioteca é organizada em 3 níveis de complexidade, do mais simples ao mais poderoso:

| Nível | Quando usar |
|---|---|
| **Fácil (Easy)** | Você só quer redimensionar rápido, sem se preocupar com detalhes |
| **Média (Medium)** | Você quer escolher tipo de dado, filtro e modo de borda |
| **Estendida (Extended)** | Você precisa de multithreading, callbacks, sub-regiões, reaproveitar cálculos entre vários resizes, etc |

Na prática, **90% dos casos** você só vai precisar da API fácil.

---

## 3. API Fácil — o que você provavelmente vai usar

```c
unsigned char * stbir_resize_uint8_srgb(
    const unsigned char *input_pixels,  int input_w,  int input_h,  int input_stride_in_bytes,
          unsigned char *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout pixel_layout);

unsigned char * stbir_resize_uint8_linear(
    const unsigned char *input_pixels,  int input_w,  int input_h,  int input_stride_in_bytes,
          unsigned char *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout pixel_layout);

float * stbir_resize_float_linear(
    const float *input_pixels,  int input_w,  int input_h,  int input_stride_in_bytes,
          float *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout pixel_layout);
```

### `srgb` vs `linear` — qual escolher?

- **`stbir_resize_uint8_srgb`**: use para imagens comuns (fotos, texturas de cor, PNGs/JPEGs do dia a dia). A maioria das imagens do mundo real é salva em espaço de cor sRGB, e essa função faz a matemática de filtragem corretamente nesse espaço — o resultado visual fica mais correto.
- **`stbir_resize_uint8_linear`**: trata os bytes como valores lineares "puros", sem nenhuma correção de gama. Use para dados que não são cor (ex: mapas de normais, dados de altura, máscaras).
- **`stbir_resize_float_linear`**: mesma ideia, mas para buffers de `float` (ex: HDR).

### Parâmetros

| Parâmetro | Significado |
|---|---|
| `input_pixels` | ponteiro para os pixels de entrada |
| `input_w`, `input_h` | largura/altura da imagem de entrada |
| `input_stride_in_bytes` | quantos bytes existem entre o início de uma linha e o início da próxima. Use **0** se os pixels estiverem "compactados" continuamente na memória (sem padding) |
| `output_pixels` | ponteiro para onde escrever o resultado. Pode ser **NULL** — nesse caso a própria função aloca o buffer de saída pra você (libere depois com `free()` ou `STBIR_FREE`) |
| `output_w`, `output_h` | largura/altura desejada da saída |
| `output_stride_in_bytes` | igual ao de entrada, mas para a saída |
| `pixel_layout` | descreve quantos canais a imagem tem e sua ordem (RGB, RGBA, BGR, etc.) — veja a próxima seção |

### Retorno

Todas retornam um ponteiro pros pixels de saída (o mesmo que você passou em `output_pixels`, ou o buffer recém-alocado se você passou `NULL`). Se der erro, retorna `NULL`.

### Exemplo simples: reduzir uma imagem RGB pela metade

```c
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize2.h"

int main(void) {
    int w, h, canais;
    unsigned char *original = stbi_load("foto.png", &w, &h, &canais, 3); // força RGB (3 canais)

    int novo_w = w / 2;
    int novo_h = h / 2;

    // stride = 0 -> pixels compactados, sem espaço extra entre linhas
    unsigned char *reduzida = stbir_resize_uint8_srgb(
        original, w, h, 0,
        NULL, novo_w, novo_h, 0,     // NULL = deixa a lib alocar a saída
        STBIR_RGB
    );

    if (!reduzida) {
        printf("Erro ao redimensionar\n");
        return 1;
    }

    // ... use "reduzida" (por exemplo, salve com stb_image_write) ...

    stbi_image_free(original);
    free(reduzida); // aqui é free() normal, pois foi a própria lib que alocou

    return 0;
}
```

---

## 4. `stbir_pixel_layout` — descrevendo o formato dos seus pixels

Esse enum diz pra biblioteca: quantos canais existem, em que ordem, e se o alfa já vem pré-multiplicado na cor.

```c
typedef enum {
  STBIR_1CHANNEL = 1,   // 1 canal (ex: escala de cinza)
  STBIR_2CHANNEL = 2,   // 2 canais genéricos (sem canal de transparência real)
  STBIR_RGB      = 3,   // 3 canais, ordem R-G-B
  STBIR_BGR      = 0,   // 3 canais, ordem B-G-R
  STBIR_4CHANNEL = 5,   // 4 canais genéricos (4º canal NÃO é tratado como transparência)

  // --- com alfa NÃO pré-multiplicado ---
  STBIR_RGBA = 4,
  STBIR_BGRA = 6,
  STBIR_ARGB = 7,
  STBIR_ABGR = 8,
  STBIR_RA   = 9,       // cinza + alfa
  STBIR_AR   = 10,

  // --- com alfa JÁ pré-multiplicado (premultiplied alpha) ---
  STBIR_RGBA_PM = 11,
  STBIR_BGRA_PM = 12,
  STBIR_ARGB_PM = 13,
  STBIR_ABGR_PM = 14,
  STBIR_RA_PM   = 15,
  STBIR_AR_PM   = 16,
} stbir_pixel_layout;
```

### Como escolher

- Se sua imagem **não tem transparência**: `STBIR_RGB`, `STBIR_BGR` ou `STBIR_1CHANNEL`.
- Se tem transparência e os valores de cor **não** foram multiplicados pelo alfa (o caso mais comum — é como PNGs normalmente são salvos, e como o Photoshop e navegadores esperam): use `STBIR_RGBA`, `STBIR_BGRA`, etc.
- Se sua imagem já vem com **alfa pré-multiplicado** (comum em pipelines de VFX/cinema): use as variantes `_PM`.
- Se o "4º canal" não representa transparência de verdade (por exemplo, você está guardando outro dado qualquer ali): use `STBIR_4CHANNEL` ou `STBIR_2CHANNEL`, que não fazem nenhum processamento especial de alfa.

> 💡 Dica de compatibilidade: em versões antigas da biblioteca (v1), você passava só "número de canais" (1, 2, 3, 4). Se está migrando um código antigo, dá pra simplesmente converter esse número pra `stbir_pixel_layout` (os valores 1-4 batem, exceto que RGBA=4 já assume não-pré-multiplicado).

### Por que o canal alfa importa tanto aqui

Ao reduzir/ampliar uma imagem, os pixels da saída são uma média ponderada de vários pixels da entrada. Se você tem um pixel verde 1% opaco do lado de um pixel preto 99% opaco, uma média "ingênua" (sem considerar o alfa) pode criar uma cor esverdeada errada que nunca existiu de fato na imagem original. Por isso a biblioteca, ao saber que aquele canal é realmente alfa, faz a "ponderação por alfa" automaticamente (basicamente pré-multiplica, filtra, depois desfaz a pré-multiplicação) — isso é mais lento, mas dá o resultado visualmente correto. Se você já sabe que sua imagem está pré-multiplicada, usar as variantes `_PM` evita esse trabalho extra e fica **~2x mais rápido**.

---

## 5. API Média — mais controle (tipo de dado, filtro, borda)

```c
void * stbir_resize(
    const void *input_pixels,  int input_w,  int input_h,  int input_stride_in_bytes,
          void *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout pixel_layout, stbir_datatype data_type,
    stbir_edge edge, stbir_filter filter);
```

Aqui você ganha controle sobre 3 coisas a mais:

### a) Tipo de dado (`stbir_datatype`)

```c
typedef enum {
  STBIR_TYPE_UINT8            = 0,  // 8 bits por canal
  STBIR_TYPE_UINT8_SRGB       = 1,  // 8 bits, tratado como sRGB
  STBIR_TYPE_UINT8_SRGB_ALPHA = 2,  // 8 bits, sRGB, e o alfa também é sRGB (raro)
  STBIR_TYPE_UINT16           = 3,  // 16 bits por canal
  STBIR_TYPE_FLOAT            = 4,  // float
  STBIR_TYPE_HALF_FLOAT       = 5   // half-float (16 bits float)
} stbir_datatype;
```

### b) Modo de borda (`stbir_edge`) — o que fazer nos limites da imagem

```c
typedef enum {
  STBIR_EDGE_CLAMP   = 0,  // repete o pixel da borda (padrão, mais rápido)
  STBIR_EDGE_REFLECT = 1,  // espelha a imagem na borda
  STBIR_EDGE_WRAP    = 2,  // "embrulha" a imagem (como um padrão que se repete) — mais lento, usa mais memória
  STBIR_EDGE_ZERO    = 3,  // trata fora da borda como preto/transparente
} stbir_edge;
```

### c) Filtro (`stbir_filter`) — algoritmo usado para calcular os novos pixels

```c
typedef enum {
  STBIR_FILTER_DEFAULT      = 0, // mesmo filtro que a API fácil escolheria
  STBIR_FILTER_BOX          = 1, // trapézio simples — bom para reduções em proporções inteiras
  STBIR_FILTER_TRIANGLE     = 2, // resultado parecido com filtro bilinear
  STBIR_FILTER_CUBICBSPLINE = 3, // spline cúbica, suave tipo "gaussiana"
  STBIR_FILTER_CATMULLROM   = 4, // spline cúbica interpolante (mais nítida)
  STBIR_FILTER_MITCHELL     = 5, // Mitchell-Netravali — bom equilíbrio nitidez/artefato
  STBIR_FILTER_POINT_SAMPLE = 6, // amostragem por ponto (vizinho mais próximo) — "pixelado", sem suavização
  STBIR_FILTER_OTHER        = 7, // você define seu próprio filtro via callback
} stbir_filter;
```

> Regra prática: `MITCHELL` costuma ser um bom filtro padrão geral. `POINT_SAMPLE` é o que você quer pra manter o visual "pixel art" sem borrar nada. `BOX` é rápido e bom quando está reduzindo por um fator inteiro (ex: metade, um terço).

### Exemplo com API média

```c
void *saida = stbir_resize(
    original, w, h, 0,
    NULL, novo_w, novo_h, 0,
    STBIR_RGBA, STBIR_TYPE_UINT8,
    STBIR_EDGE_CLAMP,
    STBIR_FILTER_MITCHELL
);
```

---

## 6. API Estendida — quando você precisa de controle total

Use essa API se precisar de:

- **Multithreading** de um único resize (dividir o trabalho entre threads).
- **Callbacks** de entrada/saída (ex: ler/escrever linha por linha, sem precisar de buffers completos na memória).
- **Sub-regiões** (redimensionar só uma parte da imagem, com precisão de subpixel).
- **Reaproveitar os cálculos internos** ("samplers") quando você vai redimensionar vários frames com as mesmas dimensões (ex: vídeo) — evita recalcular tudo a cada frame.
- Ajustar manualmente o comportamento do alfa pré-multiplicado.

### Fluxo básico de uso

```c
STBIR_RESIZE resize;

// 1) sempre inicialize primeiro
stbir_resize_init(&resize,
    original, w, h, 0,
    NULL, novo_w, novo_h, 0,
    STBIR_RGBA, STBIR_TYPE_UINT8);

// 2) (opcional) configure detalhes extras
stbir_set_edgemodes(&resize, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP);
stbir_set_filters(&resize, STBIR_FILTER_MITCHELL, STBIR_FILTER_MITCHELL);

// 3) (opcional, só se for reaproveitar em múltiplos resizes) pré-calcula os samplers
stbir_build_samplers(&resize);

// 4) executa o resize de fato
stbir_resize_extended(&resize);

// 5) se você chamou build_samplers, tem que liberar depois
stbir_free_samplers(&resize);
```

### Funções mais úteis dessa camada

| Função | Para quê serve |
|---|---|
| `stbir_resize_init` | **obrigatório** — inicializa a estrutura `STBIR_RESIZE` |
| `stbir_set_datatypes` | muda o tipo de dado de entrada/saída depois de inicializar |
| `stbir_set_pixel_layouts` | muda o layout de pixel de entrada/saída (permite até *converter* entre layouts, ex: RGBA → ARGB) |
| `stbir_set_edgemodes` | define o modo de borda separado por eixo (horizontal/vertical) |
| `stbir_set_filters` | define o filtro separado por eixo |
| `stbir_set_pixel_callbacks` | define callbacks de entrada/saída, pra processar linha por linha sem buffer completo |
| `stbir_set_pixel_subrect` / `stbir_set_input_subrect` / `stbir_set_output_pixel_subrect` | trabalha só numa sub-região da imagem |
| `stbir_set_non_pm_alpha_speed_over_quality` | troca qualidade por velocidade (~25% mais rápido) no tratamento de pixels com alfa 0 |
| `stbir_build_samplers` | pré-calcula os dados internos de amostragem (usar quando for redimensionar várias vezes com as mesmas dimensões) |
| `stbir_free_samplers` | libera o que foi alocado por `stbir_build_samplers` — **obrigatório chamar se você chamou build_samplers** |
| `stbir_resize_extended` | executa o resize de fato, numa thread só |
| `stbir_build_samplers_with_splits` + `stbir_resize_extended_split` | versão para multithreading manual |

### Multithreading manual

```c
STBIR_RESIZE resize;
stbir_resize_init(&resize, original, w, h, 0, saida, novo_w, novo_h, 0, STBIR_RGBA, STBIR_TYPE_UINT8);

int num_threads = 4;
int splits = stbir_build_samplers_with_splits(&resize, num_threads); // pode devolver menos que o pedido

// em cada thread (0, 1, 2, ... splits-1):
stbir_resize_extended_split(&resize, thread_index, 1);

// depois que todas as threads terminarem, no thread principal:
stbir_free_samplers(&resize);
```

---

## 7. Imagens invertidas (flip) sem copiar nada

Como o `stride` é só "quantos bytes até a próxima linha", você pode usar um **stride negativo** e apontar pro **último** pixel da imagem — isso efetivamente inverte a imagem verticalmente sem precisar criar uma cópia invertida na memória. Funciona tanto pra entrada quanto pra saída.

---

## 8. Conversão de layout de pixel "de graça"

Com `stbir_set_pixel_layouts` (API estendida) você pode pedir, por exemplo, entrada em `STBIR_RGBA` e saída em `STBIR_ARGB` — a biblioteca reorganiza os canais durante o próprio processo de redimensionamento. **Restrição**: só funciona entre layouts com o **mesmo número de canais**.

---

## 9. Configurações via `#define` (antes do include com a implementação)

### Alocador de memória customizado

```c
#define STBIR_MALLOC(size, user_data) meu_malloc(size)
#define STBIR_FREE(ptr, user_data)    meu_free(ptr)
```

### Desligar SIMD (ou escolher qual usar)

```c
#define STBIR_NO_SIMD      // desliga totalmente
#define STBIR_SSE2         // força SSE2 (32-bit x86)
#define STBIR_NEON         // força NEON (32-bit ARM)
#define STBIR_NO_AVX       // desliga só AVX
#define STBIR_NO_AVX2      // desliga só AVX2
#define STBIR_USE_FMA      // ativa multiply-add (mais rápido, mas menos determinístico entre CPUs)
```

### Filtros padrão (usados quando a API não pede um filtro explícito)

```c
#define STBIR_DEFAULT_FILTER_UPSAMPLE   STBIR_FILTER_CATMULLROM
#define STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL
```

### Assert customizado

```c
#define STBIR_ASSERT(cond) minha_macro_de_assert(cond)
```

### Limitar a faixa da saída em float

```c
#define STBIR_FLOAT_LOW_CLAMP  0.0f
#define STBIR_FLOAT_HIGH_CLAMP 1.0f
```

---

## 10. Coisas importantes de saber

- **Determinismo**: a biblioteca se compromete a dar o mesmo resultado em qualquer plataforma (x64, ARM, com ou sem SIMD) — desde que você compile sem "fast math" e sem contração de FP (fp-contract). Se ativar `STBIR_USE_FMA`, o resultado passa a ser determinístico apenas entre CPUs que também usam FMA.
- **Dados float**: se a entrada é float mas a saída é `uint8`/`uint16`, os valores de entrada **precisam** estar no intervalo de 0 a 1.
- **Fator de escala**: como a razão de escala é calculada internamente em float, o limite prático de ampliação/redução é de cerca de 16 milhões para 1. Para redimensionamentos muito extremos, faça em múltiplas etapas.
- **Só escala e translação**: não dá pra rotacionar nem aplicar distorção com essa biblioteca — só mudar de tamanho (e, no máximo, recortar sub-regiões).

---

## 11. Referência rápida

| Função | Camada | Para quê serve |
|---|---|---|
| `stbir_resize_uint8_srgb` | Fácil | redimensiona uint8, tratando cor como sRGB |
| `stbir_resize_uint8_linear` | Fácil | redimensiona uint8, tratando valores como lineares |
| `stbir_resize_float_linear` | Fácil | redimensiona float |
| `stbir_resize` | Média | igual à fácil, mas escolhendo tipo de dado, borda e filtro |
| `stbir_resize_init` | Estendida | inicializa a estrutura `STBIR_RESIZE` (sempre a 1ª chamada) |
| `stbir_resize_extended` | Estendida | executa o resize configurado |
| `stbir_build_samplers` / `stbir_free_samplers` | Estendida | reaproveita cálculos entre vários resizes iguais |
| `stbir_build_samplers_with_splits` / `stbir_resize_extended_split` | Estendida | multithreading manual |

---

## Licença

Assim como a `stb_image.h`, essa biblioteca é de **domínio público** (Unlicense) ou, alternativamente, sob **licença MIT** — você escolhe.
