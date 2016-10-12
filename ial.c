
#include "infinite_string.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ial.h"

bst_node_t helper_ptr; // v IAL sa da vnorovat funkcie a pristupovat k premennym materskej funkcie, preto tento skurveny balast

bool bst_search(bst_node_t root, string_t key) {
	if(root != NULL) {
		if(str_compare(root->key, key) == 0) {
			return true;
		} else {
			if(str_compare(root->key, key) > 0) {
				return bst_search(root->left_p, key);
			} else {
				return bst_search(root->right_p, key);
			}
		}
	} else {
		return false;
	}
}

bst_node_t bst_search_get(bst_node_t root, string_t key) {
	if(root == NULL) {
		return NULL;
	} else {
		if(str_compare(root->key, key) != 0) {
			if(str_compare(root->key, key) > 0) {
				return bst_search_get(root->left_p, key);
			} else {
				return bst_search_get(root->right_p, key);
			}
		} else {
			return root;
		}
	}
}

void bst_insert(bst_node_t *root_ptr, string_t key, int data) {
	if(*root_ptr == NULL) {
		_bst_create_node(root_ptr, key, data);
	} else {
		if(str_compare((*root_ptr)->key, key) > 0) {
			bst_insert(&((*root_ptr)->left_p), key, data);
		} else if(str_compare((*root_ptr)->key, key) < 0) {
			bst_insert(&((*root_ptr)->right_p), key, data);
		} else {
			(*root_ptr)->data = data;
		}
	}
}

void _bst_create_node(bst_node_t *node_ptr, string_t key, int data) {
	*node_ptr = malloc(sizeof(struct bst_node));
	(*node_ptr)->left_p = (*node_ptr)->right_p = NULL;
	(*node_ptr)->key = key;
	(*node_ptr)->data = data;
}

void bst_delete(bst_node_t *root, string_t key) {
	if(*root != NULL) {
		if(str_compare((*root)->key, key) > 0) {
			bst_delete(&((*root)->left_p), key);
		} else if(str_compare((*root)->key, key) < 0) {
			bst_delete(&((*root)->right_p), key);
		} else {
			helper_ptr = *root;
			if(helper_ptr->right_p == NULL) {
				*root = helper_ptr->left_p;
			} else if(helper_ptr->left_p == NULL) {
				*root = helper_ptr->right_p;
			} else {
				_bst_del(&(helper_ptr->left_p));
			}
			str_destroy(helper_ptr->key);
			free(helper_ptr);
			helper_ptr = NULL;
		}
	}
}

void _bst_del(bst_node_t *node) {
	if((*node)->right_p != NULL) {
		_bst_del(&((*node)->right_p));
	} else {
		helper_ptr->data = (*node)->data;
		helper_ptr->key = (*node)->key;
		helper_ptr = *node;
		*node = (*node)->left_p;
	}
}

/*procedure Del(var Uk:TUk);
(* Pomocná procedura Del se pohybuje po pravé diagonále levého podstromu rušeného uzlu a hledá nejpravější uzel. Když ho najde, je globální proměnná - ukazatel na uzel PomUk - přepsán ukazatelen na uzel Uk a Uk je připraven pro následnou operaci dispose.*) begin
	if Uk^.PUk <>nil
		Del(Uk^.PUk) (* pokračuj v pravém podstromu *)
	else begin (* nejpravější uzel je nalezen, přepsání a uvolnění uzlu *)
		PomUk^.Data:=Uk^.Data;
		PomUk^.Klic:=Uk^.Klic;
		PomUk:=Uk;
		Uk:=Uk^.LUk (* Uvolnění uzlu Uk! Pozor! V proceduře Del je Uk ukazatelová složka uzlu nadřazeného k uzlu Uk *)
	end
end; (* Konec pomocné procedure Del *)*/


/*procedure Delete(var UkKor:TUk; K:TKlic);
var
	PomUk:TUk;
begin (* tělo hlavní procedury *)
	if UkKor <> nil (* vyhledávání neskončilo; hledaný uzel může stále být v BVS *)
		if K < UkKor^.Klic
			Delete (UkKor^.LUk,K) (* pokračuj v levém podstromu *)
		else if K > UkKor^.Klic
			Delete(UkKor^.PUk,K) (* pokračuj v pravém podstromu *)
		else begin (* uzel UkKor se má rušit *)
			PomUk:=UkKor;
			if PomUk^.PUk=nil (* uzel nemá pravý podstrom; levý podstrom se připojí na nadřazený uzel *)
				UkKor:=PomUk^.LUk
			else (* rušený uzel má pravý podstrom; bude přepsán nejpravějším uzlem
				levého podstromu uvnitř procedury Del. Je-li levý podstrom prázdný, přípojí se pravý
				podstrom na nadřazený uzel *)
				if PomUk^.LUk=nil 
					UkKor:=PomUk^.PUk (* připojení pravého podstromu *)
				else
					Del(PomUk^.LUk); (* Pomocná rekurz. procedura Del *)
			dispose (PomUk); (* uvolnění uzlu *)
		end (* if K> UkKor^.LUk,K) *)
	else (* zde může být akce, když uzel nebyl nalezen; normálně se neděje nic *)
end (* delete *).*/