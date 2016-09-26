#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dict_s {
	char *word;
	struct Dict_s *next;
} Dict;

typedef struct Cipher_s {
	char *word;
	char key[26];
	Dict *match;
	int pos;
	struct Cipher_s *next;
} Cipher;


Cipher *new_cipher(char *str);

Dict *new_dict(char *str);

int get_start(char *text, int n);

int get_len(char *text, int start);

char *get_word(char *text, int start, int len );

char get_key(Cipher *head, Cipher *ptr, int key);

int already_used(Cipher *head, Cipher *ptr, char key, int current);

int match(Dict *dptr, Cipher *head, Cipher *cptr);

Cipher *get_previous(Cipher *head, Cipher *ptr);


int main(int argc, char *argv[]){
	Cipher *cipher_head=NULL, *ptrc; //initializing Cipher linked list
	int n=0;
	//cipher_head=new_cipher(get_word(argv[2],0));
	ptrc=cipher_head;
	int maxlen=0, n_of_words=1;
	while(get_start(argv[2],n)!=-1){
		if(get_len(argv[2],get_start(argv[2],n))>maxlen){
			maxlen=get_len(argv[2],get_start(argv[2],n));
		}
		n++;
	}
	while(maxlen>0){
		n=0;
		while(get_start(argv[2],n)!=-1){
			if(get_len(argv[2],get_start(argv[2],n))==maxlen && cipher_head==NULL){
				cipher_head=new_cipher(get_word(argv[2],get_start(argv[2],n),get_len(argv[2],get_start(argv[2],n))));
				cipher_head->pos=n;
				ptrc=cipher_head;
			}
			else if(get_len(argv[2],get_start(argv[2],n))==maxlen){
				ptrc->next=new_cipher(get_word(argv[2],get_start(argv[2],n),get_len(argv[2],get_start(argv[2],n))));
				ptrc=ptrc->next;
				ptrc->pos=n;
				n_of_words++;
			}
			n++;
		}
		maxlen--;
	}


	Dict *dict_head=NULL, *ptrd; //initializing Dictionary linked list
	FILE *dict;
	dict=fopen(argv[1], "r");
	char buf[20], *ret;
	memset(buf, '\0', 20);
	ret=fgets(buf, 20, dict);
	buf[strlen(buf)-1]='\0';
	while(ret!=NULL){
		if(dict_head==NULL){
			dict_head=new_dict(buf);
			ptrd=dict_head;
		}
		else{
			ptrd->next=new_dict(buf);
			ptrd=ptrd->next;
		}
		ret=fgets(buf, 20, dict);
		buf[strlen(buf)-1]='\0';
	}
	fclose(dict);
	Dict *d_run; // start solving
	Cipher *c_run;
	c_run=cipher_head;
	d_run=dict_head;
	printf("init complete\n");
	while(c_run!=NULL){
		while(d_run!=NULL && match(d_run, cipher_head, c_run)!=1){

			d_run=d_run->next;
		}
		if(d_run==NULL){
			c_run=get_previous(cipher_head, c_run);
			d_run=c_run->match->next;
		}
		else{
			printf("%s %s\n", c_run->word, d_run->word);
			c_run=c_run->next;
			d_run=dict_head;
		}
	}
	printf("done\n");
	for(int i=0; i<26; i++){ // printing key
		printf("%c: %c\n", (char)i+'a', get_key(cipher_head, get_previous(cipher_head, c_run), i));
	}
	ptrc=cipher_head;
	for(int i=0; i<n_of_words; i++){
		ptrc=cipher_head;
		while(ptrc!=NULL){
			if(ptrc->pos==i){
				printf("%s ",ptrc->match->word);
			}
			ptrc=ptrc->next;
		}
	}
	printf("\n");
	return 0;

}

Cipher *new_cipher(char *str){
	Cipher *new;
	new = (Cipher*)malloc(sizeof(Cipher));
	new->word = str;
	return new;
}

Dict *new_dict(char *str){
	Dict *new;
	new = (Dict*)malloc(sizeof(Dict));
	char *word = (char*)malloc(strlen(str)+1);
	strcpy(word, str);
	new->word=word;
	return new;
}

int get_start(char *text, int n){
	int start=0;
	while(n > 0){
		if(text[start]=='\0'){
			return -1;
		}
		if(text[start]==' '){
			n--;
		}
		start++;
	}
	return start;
}

int get_len(char *text, int start){
	int end=start;
	while(text[end] != ' ' && text[end] != '\0'){
		end++;
	}
	return end-start;
}


char *get_word(char *text, int start, int len){
	char *word=(char*)malloc(len+1);
	memset(word, '\0', len+1);
	strncpy(word, &text[start], len);
	return word;
}

char get_key(Cipher *head, Cipher *ptr, int key){
	Cipher *tmpptr = head;
	while(tmpptr!=ptr->next){
		if(tmpptr->key[key]!='\0'){
			return tmpptr->key[key];
		}
		tmpptr=tmpptr->next;
	}
	return '\0';
}

int already_used(Cipher *head, Cipher *ptr, char key, int current){
	for(int i=0; i<26; i++){
		if(i==current){
			continue;
		}
		if(key==get_key(head, ptr, i)){
			return 1;
		}
	}
	return 0;
}

int match(Dict *dptr, Cipher *head, Cipher *cptr){
	if(strlen(dptr->word) != strlen(cptr->word)){
		return 0;
	}
	memset(cptr->key, '\0', 26);
	for(int i=0; i<strlen(cptr->word); i++){
		if(get_key(head, cptr, cptr->word[i]-'a')=='\0'&& already_used(head, cptr, dptr->word[i], cptr->word[i]-'a')==0){
		       cptr->key[cptr->word[i]-'a'] = dptr->word[i];
		}
 		else if(get_key(head, cptr, cptr->word[i]-'a')!=dptr->word[i]){
			return 0;
		}
	}
	cptr->match=dptr;
	return 1;
}

Cipher *get_previous(Cipher *head, Cipher *ptr){
	Cipher *tmpptr=head;
	if(ptr==head){
		printf("unable to find key\n");
		exit(0);
	}
	while(tmpptr->next!=ptr){
		tmpptr=tmpptr->next;
	}
	return tmpptr;
}
