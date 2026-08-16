# Guia de Uso da stb_image

## O que é

`stb_image.h` é uma biblioteca em C, de arquivo único (single-file), de domínio público, para carregar imagens. Ela é muito popular no mundo dos jogos e aplicações gráficas por ser extremamente simples de integrar: basta um único header, sem dependências externas.

Formatos suportados:

- **JPEG** (baseline e progressivo)
- **PNG** (1/2/4/8/16 bits por canal)
- **BMP** (não RLE, não 1bpp)
- **TGA**
- **PSD** (apenas a visão composta)
- **GIF** (sempre retorna 4 canais)
- **HDR** (Radiance .hdr)
- **PIC** (Softimage PIC)
- **PNM** (PPM e PGM binário)

## Limitações importantes

- Não suporta JPEG de 12 bits por canal nem JPEG com codificação aritmética.
- GIF sempre reporta 4 componentes (RGBA), mesmo que a imagem original tenha menos.
- Usa `int` internamente para tamanhos, então há um limite prático de pouco menos de 2GB (ou 1GB, dependendo do formato) para a imagem decodificada.

---

## 1. Instalação (é só um header!)

Como é uma biblioteca "header-only", você não precisa compilar nada separadamente. O processo tem duas partes:

### Passo 1: em UM único arquivo .c/.cpp do seu projeto

```c
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

Isso faz o compilador gerar o código real das funções (a "implementação") **apenas nesse arquivo**.

### Passo 2: em todos os outros arquivos que só precisam usar a biblioteca

```c
#include "stb_image.h"
```

Sem o `#define`, esse include só traz as declarações das funções (o "header"), sem duplicar o código.

> ⚠️ Se você esquecer o `#define STB_IMAGE_IMPLEMENTATION` em algum arquivo, vai dar erro de "função não encontrada" (linker error). Se você colocar esse `#define` em mais de um arquivo .c, vai dar erro de "símbolo duplicado".

---

## 2. Uso básico — carregando uma imagem

```c
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
    int largura, altura, canais;

    // desired_channels = 0 significa "me dê o número de canais original da imagem"
    unsigned char *dados = stbi_load("minha_imagem.png", &largura, &altura, &canais, 0);

    if (dados == NULL) {
        printf("Erro ao carregar imagem: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Imagem carregada: %dx%d, %d canais\n", largura, altura, canais);

    // ... use os dados dos pixels aqui ...

    stbi_image_free(dados); // sempre libere a memória depois de usar!
    return 0;
}
```

### Explicando os parâmetros de `stbi_load`

```c
unsigned char *stbi_load(char const *filename,
                          int *x, int *y,
                          int *channels_in_file,
                          int desired_channels);
```

| Parâmetro | Significado |
|---|---|
| `filename` | caminho do arquivo de imagem |
| `x` | (saída) largura da imagem em pixels |
| `y` | (saída) altura da imagem em pixels |
| `channels_in_file` | (saída) quantos componentes a imagem tinha originalmente |
| `desired_channels` | quantos componentes você **quer** receber (0 = não força nada, use o original) |

Valores possíveis para `desired_channels`:

| Valor | Formato de saída |
|---|---|
| `1` | escala de cinza |
| `2` | escala de cinza + alfa |
| `3` | RGB |
| `4` | RGBA |

Mesmo se você forçar um número de canais diferente do original, `channels_in_file` sempre vai te dizer qual era o número original — útil para saber, por exemplo, se a imagem tinha transparência de verdade ou não.

### Como os pixels ficam organizados na memória

O retorno é um array de bytes (`unsigned char*`) sem nenhum preenchimento (padding) entre pixels ou linhas. O primeiro pixel do array é o **canto superior esquerdo** da imagem. Para um pixel com N componentes, os bytes ficam intercalados assim:

```
[R][G][B][A] [R][G][B][A] [R][G][B][A] ...   (para 4 canais)
[R][G][B]    [R][G][B]    [R][G][B]    ...   (para 3 canais)
```

Para acessar o pixel na posição `(px, py)` de uma imagem RGBA:

```c
int idx = (py * largura + px) * canais;
unsigned char r = dados[idx + 0];
unsigned char g = dados[idx + 1];
unsigned char b = dados[idx + 2];
unsigned char a = dados[idx + 3];
```

---

## 3. Carregando de outras formas

### De um buffer de memória (útil se você já leu o arquivo, ou baixou da internet)

```c
unsigned char *dados = stbi_load_from_memory(buffer, tamanho_buffer,
                                              &largura, &altura, &canais, 0);
```

### De um `FILE*` já aberto

```c
FILE *f = fopen("imagem.png", "rb");
unsigned char *dados = stbi_load_from_file(f, &largura, &altura, &canais, 0);
fclose(f);
```

### De callbacks de I/O personalizados

Útil se a imagem vem de um sistema de arquivos "empacotado" (ex: dentro de um .zip de assets de jogo):

```c
stbi_io_callbacks callbacks;
callbacks.read = minha_funcao_read;
callbacks.skip = minha_funcao_skip;
callbacks.eof  = minha_funcao_eof;

unsigned char *dados = stbi_load_from_callbacks(&callbacks, meu_contexto,
                                                 &largura, &altura, &canais, 0);
```

---

## 4. Verificando informações sem decodificar a imagem toda

Se você só quer saber as dimensões/formato de uma imagem sem gastar tempo decodificando-a inteira:

```c
int largura, altura, canais, ok;
ok = stbi_info("imagem.jpg", &largura, &altura, &canais);
if (ok) {
    printf("%dx%d com %d canais\n", largura, altura, canais);
}
```

Existe também `stbi_info_from_memory` e `stbi_info_from_callbacks`.

---

## 5. Imagens de 16 bits por canal

Algumas imagens (principalmente PNG) usam 16 bits por canal em vez de 8. Se você precisa dessa precisão extra:

```c
unsigned short *dados16 = stbi_load_16("imagem.png", &largura, &altura, &canais, 0);
// ... use ...
stbi_image_free(dados16);
```

Para checar se vale a pena usar a versão de 16 bits:

```c
int is16 = stbi_is_16_bit("imagem.png");
```

---

## 6. HDR (imagens em ponto flutuante)

Para imagens `.hdr` (Radiance), ou para converter qualquer imagem para float preservando faixa dinâmica:

```c
float *dados_hdr = stbi_loadf("cenario.hdr", &largura, &altura, &canais, 0);
// ... use ...
stbi_image_free(dados_hdr); // mesma função de free serve para float também
```

Para checar se um arquivo é HDR:

```c
int eh_hdr = stbi_is_hdr("arquivo.hdr");
```

Você pode ajustar como a conversão entre HDR e LDR é feita:

```c
stbi_hdr_to_ldr_gamma(2.2f);
stbi_hdr_to_ldr_scale(1.0f);

stbi_ldr_to_hdr_gamma(2.2f);
stbi_ldr_to_hdr_scale(1.0f);
```

---

## 7. Configurações úteis (flip, unpremultiply, etc.)

### Inverter verticalmente ao carregar

Muito comum em OpenGL, onde a origem da textura é o canto inferior esquerdo:

```c
stbi_set_flip_vertically_on_load(1); // 1 = ativa, 0 = desativa
```

Existe também a versão "por thread" (`stbi_set_flip_vertically_on_load_thread`), útil se você carrega imagens em múltiplas threads simultaneamente com configurações diferentes.

### PNGs exportados do iPhone (formato BGRA premultiplicado)

```c
stbi_convert_iphone_png_to_rgb(1);
stbi_set_unpremultiply_on_load(1);
```

---

## 8. Tratamento de erros

Quando o carregamento falha, a função retorna `NULL`. Para saber o motivo:

```c
unsigned char *dados = stbi_load("nao_existe.png", &largura, &altura, &canais, 0);
if (dados == NULL) {
    printf("Falha: %s\n", stbi_failure_reason());
}
```

> `stbi_failure_reason()` retorna uma mensagem bem curta e técnica, não é algo bonito para mostrar ao usuário final.

---

## 9. Liberando memória

**Sempre** libere a memória retornada por qualquer função `stbi_load*` quando não precisar mais dela:

```c
stbi_image_free(dados);
```

Internamente é só um `free()`, mas usar essa função garante compatibilidade caso você tenha definido `STBI_MALLOC`/`STBI_FREE` customizados (veja a seção seguinte).

---

## 10. Customizando a biblioteca com `#define`

Esses `#define`s devem vir **antes** do `#include "stb_image.h"` no arquivo que tem `STB_IMAGE_IMPLEMENTATION`.

### Usar seu próprio alocador de memória

```c
#define STBI_MALLOC(sz)           meu_malloc(sz)
#define STBI_REALLOC(p, newsz)    meu_realloc(p, newsz)
#define STBI_FREE(p)              meu_free(p)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

### Reduzir o tamanho do binário desabilitando formatos que você não usa

```c
#define STBI_NO_GIF
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

Ou, o caminho inverso, dizer exatamente quais formatos você quer (mais seguro para o futuro, caso novos formatos sejam adicionados na lib):

```c
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

### Remover dependência de `assert.h`

```c
#define STBI_ASSERT(x) ((void)0)
```

### Limitar o tamanho máximo de imagem (proteção contra imagens maliciosas)

```c
#define STBI_MAX_DIMENSIONS (1 << 20) // por exemplo, limita a 1048576 pixels por lado
```

### Rodar sem `<stdio.h>` (ex: em ambientes embarcados)

```c
#define STBI_NO_STDIO
```
Isso remove as funções que recebem `filename` ou `FILE*` — nesse caso você só pode carregar via memória (`stbi_load_from_memory`) ou callbacks.

---

## 11. Exemplo completo: carregar e salvar em um vetor RGBA

```c
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>

int main(void) {
    int w, h, n;

    // força a saída como RGBA (4 canais), independente do formato original
    unsigned char *img = stbi_load("foto.jpg", &w, &h, &n, 4);
    if (!img) {
        fprintf(stderr, "Erro: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Carregado %dx%d (originalmente %d canais)\n", w, h, n);

    // exemplo: ler o pixel central
    int cx = w / 2, cy = h / 2;
    int idx = (cy * w + cx) * 4;
    printf("Pixel central: R=%d G=%d B=%d A=%d\n",
           img[idx], img[idx+1], img[idx+2], img[idx+3]);

    stbi_image_free(img);
    return 0;
}
```

---

## 12. Referência rápida das principais funções

| Função | Para quê serve |
|---|---|
| `stbi_load` | carrega de um arquivo (8 bits/canal) |
| `stbi_load_from_memory` | carrega de um buffer em memória |
| `stbi_load_from_file` | carrega de um `FILE*` já aberto |
| `stbi_load_from_callbacks` | carrega via callbacks de I/O customizados |
| `stbi_load_16` / `*_16_from_memory` / etc. | mesmas variantes, mas em 16 bits/canal |
| `stbi_loadf` / `*_from_memory` / etc. | mesmas variantes, mas em float (HDR) |
| `stbi_info` / `*_from_memory` / etc. | pega dimensões/canais sem decodificar tudo |
| `stbi_is_hdr` | verifica se o arquivo é HDR |
| `stbi_is_16_bit` | verifica se a imagem é de 16 bits/canal |
| `stbi_image_free` | libera a memória retornada por qualquer `stbi_load*` |
| `stbi_failure_reason` | retorna a última mensagem de erro |
| `stbi_set_flip_vertically_on_load` | inverte a imagem verticalmente ao carregar |
| `stbi_convert_iphone_png_to_rgb` | converte PNGs do formato iPhone (BGRA) para RGB |
| `stbi_set_unpremultiply_on_load` | desfaz alfa premultiplicado quando aplicável |

---

## 13. Dica de compilação (GCC/Clang)

```bash
gcc meu_programa.c -o meu_programa -lm
```

O `-lm` é necessário porque a biblioteca usa funções matemáticas (`pow`, `ldexp`) em alguns caminhos (conversão HDR, por exemplo).

---

## Licença

`stb_image.h` é distribuída como **domínio público** (Unlicense) ou, alternativamente, sob **licença MIT** — você escolhe qual das duas prefere seguir. Não há necessidade de dar créditos, mas é uma cortesia comum mencionar a biblioteca original de Sean Barrett.
