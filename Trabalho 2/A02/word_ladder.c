//
// AED, November 2022 (Tomás Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. 107188  Name: 105926
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create --> done
//      *) hash_table_grow --> done
//      *) hash_table_free --> done
//      *) find_word --> done
//      +) add code to get some statistical data about the hash table --> >TODO
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative --> done
//      *) add_edge --> done
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search --> done
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search --> done
//      *) list_connected_component --> done
//   5) RECOMMENDED: find the shortest path between to words
//      *) breadh_first_search --> done
//      *) path_finder --> done
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders
//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search --> done
//      *) connected_component_diameter --> done
//   7) OPTIONAL: print some statistics about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// #define malloc(s)  ({ void *m = malloc(s); printf("M %3d %016lX %ld\n",__LINE__,(long)m,(long)(s)); m; })
// #define free(m)    ({ printf("F %3d %016lX\n",__LINE__,(long)m); free(m); })

//
// static configuration
//

#define _max_word_size_  32


//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s  adjacency_node_t; 
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s      hash_table_t;

struct adjacency_node_s\
{
  adjacency_node_t *next;            // link to th enext adjacency list node
  hash_table_node_t *vertex;         // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_];        // the word
  hash_table_node_t *next;           // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;            // head of the linked list of adjancency edges
  int visited;                       // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous;       // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)

  // added for graph information
  int biggest_diameter_of_cc;
};

struct hash_table_s
{
  unsigned int hash_table_size;      // the size of the hash table array
  unsigned int number_of_entries;    // the number of entries in the hash table
  unsigned int number_of_edges;      // number of edges (for information purposes only)
  hash_table_node_t **heads;         // the heads of the linked lists
};

static int size;

//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}


//
// hash table stuff (mostly to be done)
//

unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;

  if(table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i,j;

    for(i = 0u;i < 256u;i++)
      for(table[i] = i,j = 0u;j < 8u;j++)
        if(table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while(*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}

static hash_table_t *hash_table_create(void)
{
  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
  if(hash_table == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }
  //
  // complete this
  //
  hash_table->number_of_entries = 0;
  hash_table->hash_table_size = 100; // tamanho escolhido pois é o tamanho do file mais pequeno
  hash_table->number_of_edges = 0;
  hash_table->heads = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size); // como é um array de ponteiros é ponteiro para ponteiro
  if(hash_table == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }

  for(i = 0; i < hash_table->hash_table_size; i++)
  {
    hash_table->heads[i] = NULL; // deixar todos os heads a NULL para poder serem preenchidos
    // printf("%s", hash_table->heads[i]->word); // ok faz sentido ne, agora, usar isto na main perceber o que se esta a passar e tentar por o grow 
  }

  return hash_table;
}

static void hash_table_grow(hash_table_t *hash_table) // quando a tabela cresce, os itens ate entao colocados precisam de ser reentrados
{
  //
  // complete this
  //
  unsigned int old_size, i, j;
  static hash_table_node_t **old_heads;
  hash_table_node_t *n, *nn;

  old_size = hash_table->hash_table_size;
  old_heads = hash_table->heads;

  hash_table->hash_table_size *= 1.5 + 1; // incrementa mais metade do tamanho
  hash_table->heads = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size); // como é um array de ponteiros é ponteiro para ponteiro
  for(i = 0; i < hash_table->hash_table_size; i++)
    hash_table->heads[i] = NULL;

  for(i = 0; i < old_size; i++)
    for(n = old_heads[i]; n != NULL; n = nn)
    {
      nn = n->next;
      j = crc32(n->word) % hash_table->hash_table_size;
      n->next = hash_table->heads[j];
      hash_table->heads[j] = n;
      old_heads[i] = NULL; // para conseguir dar free
    }
  free(old_heads);
}

static void hash_table_free(hash_table_t *hash_table)
{
  unsigned int i;
  hash_table_node_t* temp; 
  adjacency_node_t* adj_temp;
  //
  // complete this
  //
  
  for(i = 0; i < hash_table->hash_table_size; i++) // para limpar a hashtable
  {
    temp = hash_table->heads[i]; // para o inicio daquele indice
    while(temp != NULL)
    {
      adj_temp = temp->head;
      while(adj_temp != NULL)
      {
        temp->head = temp->head->next;
        free_adjacency_node(adj_temp);
        adj_temp = temp->head;
      }
      hash_table->heads[i] = hash_table->heads[i]->next; // avança para o proximo para tambem o libertar os nodes
      free_hash_table_node(temp);
      temp = hash_table->heads[i];
    }
  }
  free(hash_table->heads);
  free(hash_table);
}

static hash_table_node_t *find_word(hash_table_t *hash_table,const char *word,int insert_if_not_found){
  // hash_table_node_t *head_cpy, *prev;
  hash_table_node_t *node; // é o principal que possui tudo

  // we need to allocate memory
  unsigned int i;

  i = crc32(word) % hash_table->hash_table_size;
  for(node = hash_table->heads[i];node != NULL && strcmp(node->word,word) != 0;node = node->next)
    ;
  if(node == NULL && insert_if_not_found != 0)
  {
      node = allocate_hash_table_node();
      strcpy(node->word, word);
      node->next = hash_table->heads[i];
      hash_table->heads[i] = node;
      node->representative = node;
      node->visited = 0;
      node->previous = NULL; // nao possui anteriores pois é o primeiro
      node->number_of_vertices = 1;
      node->number_of_edges = 0;
      node->biggest_diameter_of_cc = 0;
      hash_table->number_of_entries = hash_table->number_of_entries + 1;
  }
  if(hash_table->number_of_entries*2 > hash_table->hash_table_size){ // se a tabela ja estiver meio cheia
    hash_table_grow(hash_table);
  }
   return node;
}


//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *n1,*n2, *n3;
  
  //
  // complete this
  //
  for(n1 = node; n1->representative != n1; n1 = n1->representative)
    ;
  for(n2 = node; n2 != n1; n2 = n3)
  {
    n3 = n2->representative;
    n2->representative = n1;
  }
  return n1;
}

static void add_edge(hash_table_t *hash_table,hash_table_node_t *from,const char *word)
{

  hash_table_node_t *to,*from_representative,*to_representative;
  adjacency_node_t *link;

  //
  // complete this
  //

  to = find_word(hash_table,word,0);
  if(to == NULL) // word nao existe
    return;

  hash_table->number_of_edges++;

  from_representative = find_representative(from);
  to_representative = find_representative(to);
  
  // printf("%s  %s\n", from->word, to->word); // to get information for matlab graph

  // temos de criar as ligações entre o from e o to
  // from
  link = allocate_adjacency_node(); // reserva a memoria para poder ser usado
  link->vertex = to;
  link->next = from->head;
  from->head = link;

  // to
  link = allocate_adjacency_node(); // reserva a memoria para poder ser usado
  link->vertex = from;
  link->next = to->head;
  to->head = link;

  if(from_representative == to_representative) // têm o mesmo representativo logo so precisava da ligação
    return;

  if(from_representative->number_of_vertices < to_representative->number_of_vertices)
  {
    to_representative->number_of_edges = to_representative->number_of_edges + 1 + from_representative->number_of_edges;
    to_representative->number_of_vertices += from_representative->number_of_vertices;
    from_representative->representative = to_representative;
  }
  else{
    from_representative->number_of_edges = from_representative->number_of_edges + 1 + to_representative->number_of_edges;
    from_representative->number_of_vertices += to_representative->number_of_vertices;
    to_representative->representative = from_representative;
  }
}


//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word,int *individual_characters)
{
  int byte0,byte1;

  while(*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if(byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr,"break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters,char word[_max_word_size_])
{
  int code;

  while(*individual_characters != 0)
  {
    code = *(individual_characters++);
    if(code < 0x80)
      *(word++) = (char)code;
    else if(code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr,"make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0';  // mark the end
}

static void similar_words(hash_table_t *hash_table,hash_table_node_t *from)
{
  static const int valid_characters[] =
  { // unicode!
    0x2D,                                                                       // -
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,           // A B C D E F G H I J K L M
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,           // N O P Q R S T U V W X Y Z
    0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,           // a b c d e f g h i j k l m
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,           // n o p q r s t u v w x y z
    0xC1,0xC2,0xC9,0xCD,0xD3,0xDA,                                              // Á Â É Í Ó Ú
    0xE0,0xE1,0xE2,0xE3,0xE7,0xE8,0xE9,0xEA,0xED,0xEE,0xF3,0xF4,0xF5,0xFA,0xFC, // à á â ã ç è é ê í î ó ô õ ú ü
    0
  };
  int i,j,k,individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word,individual_characters);
  for(i = 0;individual_characters[i] != 0;i++)
  {
    k = individual_characters[i];
    for(j = 0;valid_characters[j] != 0;j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters,new_word);
      // avoid duplicate cases
      if(strcmp(new_word,from->word) > 0)
        add_edge(hash_table,from,new_word);
    }
    individual_characters[i] = k;
  }
}


//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

static int breadh_first_search(int maximum_number_of_vertices,hash_table_node_t **list_of_vertices,hash_table_node_t *origin,hash_table_node_t *goal)
{
  //
  // complete this
  //

  // supondo que os vertices ja estao no list_of_vertices:
  int write, read, found_flag, count, clear;
  adjacency_node_t *node;
  hash_table_node_t *go_back_node;

  write = 1; read = 0, found_flag = 0;
  origin->visited = 1;
  origin->previous = NULL; // garantir que esta NULL para fazer o search mais tarde
  list_of_vertices[0] = origin; // o primeiro é o from e vamos procurar o to
  node = list_of_vertices[0]->head; // a primeira palavra adjacente á origin
  
  while(read < maximum_number_of_vertices) // para ler todas as palavras na lista se necessário
  {
    while(node != NULL)
    {
      if(node->vertex->visited == 1) // ja esteve neste node, nao vai ca estar again
      {
        node = node->next;
        continue;
      }

      node->vertex->visited = 1; // foi visitado;
      node->vertex->previous = list_of_vertices[read]; // definir o node anterior para mais tarde conseguir descobrir o caminnho
      list_of_vertices[write] = node->vertex;
      if(node->vertex == goal) // encontramos a solução
      {
        found_flag = 1;
        break;
      }
      write++;
      node = node->next; // avança para o proximo adjacnete
    }
    // vai sempre encontrar algures uma vez que a palavra esta no mesmo componente conexo
    if(found_flag == 1) // encontrou o goal --> é o node->vertex
    {
      // limpar os visited
      for(clear = 0; clear <= write; clear++)
        list_of_vertices[clear]->visited = 0;

      // determinar o pretendido
      count = 1; // começa com um que é no que está
      go_back_node = node->vertex; // node que vai sendo atualizado com o previous para encontrar o caminho de volta
      while(go_back_node->previous != NULL){
        count++;
        go_back_node = go_back_node->previous;
      }
      return count;
    }
    read++; // avança para o proximo elemento a ler --> seguinfo as regras deste
    if(list_of_vertices[read] == NULL)
      break;

    node = list_of_vertices[read]->head;
  }
  for(clear = 0; clear < write; clear++)
    list_of_vertices[clear]->visited = 0;
  return -1;
}


//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table,const char *word) //nao sei se ja esta --> nem esta nem a de cima
{
  //
  // complete this
  //
  // usar o array dado pelo bfs inteiro
  hash_table_node_t **list_of_vertices;
  int maximum_number_of_vertices;
  unsigned int i;
  hash_table_node_t *origin, *goal, *node;
  
  list_of_vertices = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size);
  maximum_number_of_vertices = hash_table->hash_table_size;
  origin = find_word(hash_table, word, 0);
  if(origin == NULL)
    return;
  goal = NULL;

  breadh_first_search(maximum_number_of_vertices, list_of_vertices, origin, goal);

  i = 0;
  node = list_of_vertices[i];
  while(node != NULL)
  {
    printf("%s\n", node->word);
    list_of_vertices[i] = NULL;
    i++;
    node = list_of_vertices[i];
  }
  free(list_of_vertices);
}


//
// compute the diameter of a connected component (optional)
//

static int largest_diameter;
static hash_table_node_t **largest_diameter_example;

static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;
  // array completo como o de cima e vai ver quantos previous tem do ultimo até ao primeiro --> assim da o maior
  // com o representante talvez
  //
  // complete this
  //
  hash_table_node_t **list_of_vertices;
  int maximum_number_of_vertices;

  list_of_vertices = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * size); // size é um static int definido em cima e que obtem o valor de hash_table_size na main
  maximum_number_of_vertices = size;

  breadh_first_search(maximum_number_of_vertices, list_of_vertices, node, NULL); // vai ter o maior caminho em nós logo é so ver ate ao final

  int i = 0;
  while(list_of_vertices[i] != NULL){
    node = list_of_vertices[i]; // usar o node agora para guardar o valor do último nó do array
    list_of_vertices[i] = NULL; // limpar o array
    i++;
  }

  diameter = 0;
  while(node->previous != NULL)
  {
    diameter++;
    node = node->previous;
  }

  free(list_of_vertices); // libertar a memoria usada pelo array
  return diameter;
}


//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table,const char *from_word,const char *to_word)
{
  //
  // complete this
  //
  // acho que basta usar os previous do to ate ao from porque o bfs ja o faz
  hash_table_node_t **list_of_vertices, *from, *to, *from_representative, *to_representative;
  int maximum_number_of_vertices, count, word_number;

  list_of_vertices = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size);
  maximum_number_of_vertices = hash_table->hash_table_size;

  from = find_word(hash_table, from_word, 0);
  to = find_word(hash_table, to_word, 0);
  if(from == NULL || to == NULL)
  {
    printf("Palavra inexistente\n");
    return;
  }
  from_representative = find_representative(from);
  to_representative = find_representative(to);
  // printf("%s == %s\n", from_representative->word, to_representative->word);
  if(from_representative != to_representative)
  {
    printf("Palavras introduzidas não apresentam word ladder\n");
    return;
  }
  breadh_first_search(maximum_number_of_vertices, list_of_vertices, from, to);
  hash_table_node_t **previous_words = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size);
  count = 0;

  while(to != NULL)
  {
    previous_words[count] = to;
    to = to->previous;
    count++;
  }

  word_number = 0;
  count--;
  while(count >= 0)
  {
    printf("[%d] %s\n", word_number, previous_words[count]->word);
    count--;
    word_number++;
  }
  free(list_of_vertices); free(previous_words);
}


//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  //
  // complete this
  //
  hash_table_node_t **representatives = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size);
  largest_diameter_example = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size);
  hash_table_node_t *node, *node_representative, *goal;
  unsigned int i, h;
  int cc, vertices, j, diameter, store_ld_index, ind, all_diameter_counter;
  cc = 0, vertices = 0, largest_diameter = 0, store_ld_index = -1, ind = 0;
  goal = NULL;

  for(i = 0u;i < hash_table->hash_table_size;i++){
    for(node = hash_table->heads[i];node != NULL;node = node->next){
      node_representative = find_representative(node);

      for(h = 0u; h < hash_table->hash_table_size; h++){  // para guardar o numero de cc e tudo mais
        if(node_representative == representatives[h])
          break;

        if(h == hash_table->hash_table_size - 1){// caso ainda nao tenha visto o representante
          representatives[cc] = node_representative;
          cc++;
          node_representative->visited = 1;
          vertices += node_representative->number_of_vertices;
        }
      }

      diameter = connected_component_diameter(node); // encontra o diametro daquele cc
      largest_diameter_example[ind] = node;

      if(diameter > largest_diameter){
          largest_diameter = diameter;
          store_ld_index = ind;
      }

      if(diameter > node_representative->biggest_diameter_of_cc)
        node_representative->biggest_diameter_of_cc = diameter;

      ind++;
    }
  }

  int all_diameters[largest_diameter+1];
  for(int a = 0; a < largest_diameter+1; a++)
    all_diameters[a] = 0;
  
  // vertices, edges, cc's uma cena assim // maximo vertices e edces num determinado cc
  printf("Number of connected commponents: %d\n", cc);
  printf("Number of vertices: %d\n", vertices);
  printf("Number of edges: %d\n", hash_table->number_of_edges);
  printf("Largest Diameter: %d\n", largest_diameter);
  hash_table_node_t **list_of_vertices = list_of_vertices = (hash_table_node_t **)malloc(sizeof(hash_table_node_t) * hash_table->hash_table_size);
  int maximum_number_of_vertices = hash_table->hash_table_size;
  breadh_first_search(maximum_number_of_vertices, list_of_vertices, largest_diameter_example[store_ld_index], NULL);
  int count = 0;
  while (list_of_vertices[count] != NULL)
  {
    goal = list_of_vertices[count]; // tem o intuito de guardar o ultimo
    list_of_vertices[count] = NULL; // vai limpando
    count++;
  }
  printf("Longest word ladder:\n");
  path_finder(hash_table, largest_diameter_example[store_ld_index]->word, goal->word);

  j = 0;
  while(representatives[j] != NULL){
    all_diameters[representatives[j]->biggest_diameter_of_cc] += 1;
    representatives[j]->visited = 0;
    largest_diameter_example[j] = NULL; // têm ambos o mesmo comprimento
    j++;
  }

  printf("Number of connected components with a diameter of:\n");
  for(all_diameter_counter = 0; all_diameter_counter < largest_diameter+1; all_diameter_counter++){
    if(all_diameters[all_diameter_counter] != 0)
      printf("%d: %d\n", all_diameter_counter, all_diameters[all_diameter_counter]);
  }

  free(representatives); free(largest_diameter_example); free(list_of_vertices);
}


//
// main program
//

int main(int argc,char **argv)
{
  char word[100],from[100],to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;

  // initialize hash table
  hash_table = hash_table_create();
  // read words
  fp = fopen((argc < 2) ? "wordlist-big-latest.txt" : argv[1],"rb");
  if(fp == NULL)
  {
    fprintf(stderr,"main: unable to open the words file\n");
    exit(1);
  }

  while(fscanf(fp,"%99s",word) == 1){
    (void)find_word(hash_table,word,1);
  }
  // int num_words = 0;
  // for(i = 0u;i < hash_table->hash_table_size;i++){
  //   for(node = hash_table->heads[i];node != NULL;node = node->next){
  //     num_words++;
  //     printf("%d-> %s;%d --- ", i, node->word, num_words);
  //   }
  //   printf("\n");
  // }
  
  // printf("%d", hash_table->hash_table_size);
  fclose(fp);

  size = hash_table->hash_table_size;
  // find all similar words
  for(i = 0u;i < hash_table->hash_table_size;i++)
    for(node = hash_table->heads[i];node != NULL;node = node->next)
      similar_words(hash_table,node);

  // for(i = 0u;i < hash_table->hash_table_size;i++){
  //   for(node = hash_table->heads[i];node != NULL;node = node->next){
  //     printf("%s --> ", node->word);
  //     while(node->head != NULL){
  //       printf("%s -- ", node->head->vertex->word);
  //       node->head = node->head->next;
  //     }
  //     printf("\n");
  //   }
  // }

  graph_info(hash_table);
  // ask what to do
  for(;;)
  {
    fprintf(stderr,"Your wish is my command:\n");
    fprintf(stderr,"  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr,"  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr,"  3            (terminate)\n");
    fprintf(stderr,"> ");
    if(scanf("%99s",word) != 1)
      break;
    command = atoi(word);
    if(command == 1)
    {
      if(scanf("%99s",word) != 1)
        break;
      list_connected_component(hash_table,word);
    }
    else if(command == 2)
    {
      if(scanf("%99s",from) != 1)
        break;
      if(scanf("%99s",to) != 1)
        break;
      path_finder(hash_table,from,to);
    }
    else if(command == 3)
      break;
  }
  // clean up
  hash_table_free(hash_table);
  return 0;
}