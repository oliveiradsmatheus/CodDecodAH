struct bits {
	unsigned char b7:1; // bit menos significativo
	unsigned char b6:1;
	unsigned char b5:1;
	unsigned char b4:1;
	unsigned char b3:1;
	unsigned char b2:1;
	unsigned char b1:1;
	unsigned char b0:1; // bit mais significativo
};

union byte {
	struct bits bi;
	unsigned char num;
};
typedef union byte Byte;

struct tree {
	int freq;
	char num;
	struct tree *esq, *dir;
};
typedef struct tree Tree;

struct floresta {
	Tree *no;
	struct floresta *prox;
};
typedef struct floresta Floresta;

struct tabela {
	char palavra[30], codigo[8];
	int num, freq;
};
typedef struct tabela Tab;

struct lista {
	Tab Tab;
	struct lista *prox;
};
typedef struct lista Lista;

Lista *NovaCaixa (char *palavra, int num) {
	Lista *NC = (Lista*)malloc(sizeof(Lista));

	strcpy(NC->Tab.palavra,palavra);
	NC->Tab.num = num;
	NC->Tab.freq = 1;
	NC->prox = NULL;

	return NC;
}

char EhFolha (Tree *no) {
	if(!no->esq && !no->dir)
		return 1;
	return 0;
}

int QtdeEsquerda (Tree *raiz) {
	int cont = 0;

	if(raiz)
		while(raiz->esq) {
			raiz = raiz->esq;
			cont++;
		}

	return cont;
}

int QtdeDireita (Tree *raiz) {
	int cont = 0;

	if(raiz)
		while(raiz->dir) {
			raiz = raiz->dir;
			cont++;
		}

	return cont;
}

void QtdeFilhos (Tree *raiz, int *cont) {
	if(raiz) {
		if(raiz->esq) {
			(*cont)++;
			QtdeFilhos(raiz->esq,&*cont);
		}
		if(raiz->dir) {
			(*cont)++;
			QtdeFilhos(raiz->dir,&*cont);
		}
	}
}

void ExibeVertical (Tree *raiz, int c, int l) {
	int i, filhos = 0, QtdeEsq = 0, QtdeDir = 0;
	
	if(raiz) {
		if(raiz->esq) {
			filhos = 1;
			QtdeFilhos(raiz->esq,&filhos);
		}
		c+=filhos;
		
		gotoxy(c*3,l);
		printf("(%d)\n",raiz->freq,c);
		ExibeVertical(raiz->esq,c-filhos,l+1);
		ExibeVertical(raiz->dir,c+1,l+1);
	}
}

void ExibeHorizontal (Tree *raiz) {
	int i;
	static int n=-1;

	if(raiz) {
		n++;
		ExibeHorizontal(raiz->dir);
		for(i=0; i<5*n; i++)
			printf(" ");
		printf("(%d)\n", raiz->freq);
		ExibeHorizontal(raiz->esq);
		n--;
	}
}

void ExibeLista (Lista *L) {
	while(L) {
		printf("<%s - [%d] - %d - %s>\n", L->Tab.codigo, L->Tab.num, L->Tab.freq, L->Tab.palavra);
		L = L->prox;
	}
}

char BuscaRecorrencia (Lista *L, char *palavra) {
	while(L && strcmp(L->Tab.palavra,palavra))
		L = L->prox;
	if(L)
		return 1;
	return 0;
}

void SomaFreq (Lista *L, char *palavra) {
	while(L && strcmp(L->Tab.palavra,palavra))
		L = L->prox;
	L->Tab.freq++;
}

void InsereLista (Lista **L, char *palavra, int num) {
	Lista *Nova, *aux;

	if(!*L) {
		Nova = NovaCaixa(palavra, num);
		*L = Nova;
	} else {
		aux = *L;
		while(aux->prox && strcmp(aux->Tab.palavra,palavra))
			aux = aux->prox;
		Nova = NovaCaixa(palavra,num);
		aux->prox = Nova;
	}
}

void ProcuraPalavras (Lista **L, char *string) {
	char palavra[30];
	int i=0, j=0, num=0;

	while(i<strlen(string)) {
		if(string[i] != ' ') {
			palavra[j++] = string[i];
		} else {
			if(!BuscaRecorrencia(*L," ")) {
				InsereLista(&*L," ",num);
				num++;
			} else
				SomaFreq(*L," ");
			palavra[j] = '\0';
			if(!BuscaRecorrencia(*L,palavra)) {
				InsereLista(&*L,palavra,num);
				num++;
			} else
				SomaFreq(*L,palavra);
			j=0;
			palavra[0] = '\0';
		}
		i++;
	}
	palavra[j] = '\0';
	InsereLista(&*L,palavra,num);
}

Tree *CriaNoTab (Tab T) {
	Tree *NC = (Tree*)malloc(sizeof(Tree));

	NC->dir = NC->esq = NULL;
	NC->num = T.num;
	NC->freq = T.freq;

	return NC;
}

Tree *CopiaNo (Tree *T) {
	Tree *NC = (Tree*)malloc(sizeof(Tree));

	NC->dir = T->dir;
	NC->esq = T->esq;
	NC->num = T->num;
	NC->freq = T->freq;

	return NC;
}

Floresta *NovaArvore (Tree *T) {
	Floresta *NC = (Floresta*)malloc(sizeof(Floresta));

	NC->no = T;
	NC->prox = NULL;

	return NC;
}

void InsereFloresta (Floresta **F, Tree *T) {
	Floresta *aux, *Novo = NovaArvore(T);

	if(!*F)
		*F = Novo;
	else {
		if(Novo->no->freq <= (*F)->no->freq) {
			Novo->prox = *F;
			*F = Novo;
		} else {
			aux = *F;
			while(aux->prox && Novo->no->freq > aux->prox->no->freq)
				aux = aux->prox;
			Novo->prox = aux->prox;
			aux->prox = Novo;
		}
	}
}

void CriaFloresta (Floresta **F, Lista *L) {
	Tree *Novo;
	while(L) {
		Novo = CriaNoTab(L->Tab);
		InsereFloresta(&*F,Novo);
		L = L->prox;
	}
}

void ArvoreHuffman (Floresta **F) {
	Tree *Novo;
	Floresta *pri, *seg;

	while((*F)->prox) {
		Novo = (Tree*)malloc(sizeof(Tree));
		pri = *F;
		seg = (*F)->prox;
		Novo->esq = pri->no;
		Novo->dir = seg->no;
		Novo->num = -1;
		Novo->freq = pri->no->freq + seg->no->freq;
		*F = (*F)->prox->prox;
		free(pri);
		free(seg);
		InsereFloresta(&*F,Novo);
	}
}

void GravaTabela (Lista *L) {
	FILE *Arq = fopen("Data\\tabela.dat","wb");
	Tab R;
	while(L) {
		R = L->Tab;
		fwrite(&R,sizeof(Tab),1,Arq);
		L = L->prox;
	}
	fclose(Arq);
}

void BuscaCodigo (char *palavra, Lista *L, char *codigo) {
	while(L && strcmp(palavra,L->Tab.palavra))
		L = L->prox;
	if(L)
		strcpy(codigo,L->Tab.codigo);
}

void CorrigeTamanho (char *codigo) {
	int resto = strlen(codigo) % 8, qtde = 0;
	
	if(resto)
		qtde = 8-resto;
	while(qtde--)
		strcat(codigo,"0");
}

void CodificaTexto (char *texto, Lista *L, char *codigo) {
	char palavra[30], aux[10];
	int i=0, j;
	gotoxy(1,1);
	
	while(i < strlen(texto)) {
		strcpy(palavra,"");
		if(texto[i] == ' ') {
			BuscaCodigo(" ",L,aux);
			strcat(codigo,aux);
		} else {
			j=0;
			while(i < strlen(texto) && texto[i] != ' ') {
				palavra[j] = texto[i];
				i++;
				j++;
			}
			i--;
			palavra[j] = '\0';
			BuscaCodigo(palavra,L,aux);
			strcat(codigo,aux);
		}
		i++;
	}
	
	CorrigeTamanho(codigo);
}

void GravaCodigo (char *codigo) {
	Byte B;
	FILE *arq = fopen("Data\\frase.dat","wb");
	int i=0;
	
	gotoxy(1,1);
	
	while(i < strlen(codigo)) {
		B.bi.b7 = codigo[i] - '0';
		B.bi.b6 = codigo[i + 1] - '0';
		B.bi.b5 = codigo[i + 2] - '0';
		B.bi.b4 = codigo[i + 3] - '0';
		B.bi.b3 = codigo[i + 4] - '0';
		B.bi.b2 = codigo[i + 5] - '0';
		B.bi.b1 = codigo[i + 6] - '0';
		B.bi.b0 = codigo[i + 7] - '0';
		fwrite(&B,sizeof(Byte),1,arq);
		i+=8;
	}
	
	fclose(arq);
}
