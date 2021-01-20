
import copy;

def SBox(input):
    A = (input >> 0) & 1;
    B = (input >> 1) & 1;
    C = (input >> 2) & 1;
    D = (input >> 3) & 1;
    tmpA = (A&B)^C;
    tmpC = (B|C)^D;
    tmpD = (tmpA&D)^A;
    tmpB = (tmpC&A)^B;
    ret = tmpA+(tmpB<<1)+(tmpC<<2)+(tmpD<<3);
    return ret;

# Réduction avec le Polynome (X^4+X+1).
def multGF16(A, B):
    ret = 0 ;
    while(A > 0):
        ret ^= (B*(A&1));
        A >>= 1;
        B <<= 1;
        if (B&0b10000):
            B ^= 0b10011;
    return ret;

def powGF16(A, n):
    ret=0b0001;
    while(n>0):
        if n&1:
            ret = multGF16(ret, A)
        A = multGF16(A, A);
        n >>= 1;
    return ret;

def invGF16(A):
    return powGF16(A,14);

def print_state(A):
    for i, a in enumerate(A):
        print(i, ".", bin(a)[2:].zfill(32));

def gauss(current_matrix):
    current_matrix = copy.deepcopy(current_matrix);
    nb_row = len(current_matrix);
    nb_column = len(current_matrix[0]);
    ret = [[0 for _ in range(nb_column)] for _ in range(nb_row)];
    for i in range(nb_row):
        ret[i][i] = 0b0001;
    for i in range(nb_row):
        # first, find a non-null pivot
        row_candidate_index = -1;
        for j in range(i, nb_row):
            if (current_matrix[j][i] != 0):
                row_candidate_index = j;
                break;
        # if a pivot has been found
        if row_candidate_index >= 0:
            # switch rows
            current_matrix[i], current_matrix[row_candidate_index] = current_matrix[row_candidate_index], current_matrix[i];
            ret[i], ret[row_candidate_index] = ret[row_candidate_index], ret[i];
            # get pivot value ;
            pivot_value = invGF16(current_matrix[i][i]);
            for j in range(nb_column):
                current_matrix[i][j] = multGF16(current_matrix[i][j], pivot_value);
                ret[i][j] = multGF16(ret[i][j], pivot_value);
            for j in range(i+1, nb_row):
                coeff = current_matrix[j][i];
                for k in range(nb_column):
                    current_matrix[j][k] ^= multGF16(coeff, current_matrix[i][k]);
                    ret[j][k] ^= multGF16(coeff, ret[i][k]);
        # Remontée
        for i in range(nb_row-1, 0, -1):
            for j in range(i):
                coeff = current_matrix[j][i];
                for k in range(nb_column):
                    current_matrix[j][k] ^= multGF16(current_matrix[i][k], coeff);
                    ret[j][k] ^= multGF16(ret[i][k], coeff);
    return ret;

def matMult(A,B):
    nb_row = len(B);
    nb_column = len(A[0]);
    ret = [[0 for _ in range(nb_column)] for _ in range(nb_row)];
    for i in range(nb_row):
        for j in range(nb_column):
            tmp = 0b000;
            for k in range(nb_row):
                tmp ^= multGF16(A[i][k], B[k][j]);
            ret[i][j] = tmp;
    return ret;

def vecMult(A, v):
    nb_row = len(v);
    nb_column = len(A);
    ret = [0 for _ in range(nb_column)];
    for i in range(nb_column):
        tmp = 0b000;
        for k in range(nb_row):
            tmp ^= multGF16(A[i][k], v[k]);
        ret[i] = tmp;
    return ret;

# transpose the given matrix A.
def t(A):
    ret = [[A[i][j] for i in range(len(A))] for j in range(len(A[0]))]
    return ret;

if __name__ == "__main__":
#########################
# Calcul des constantes #
#########################

    Puissances = [[0, 3, 4, 12, 8, 12, 4, 3],
    [3, 13, 4, 1, 0, 2, 2, 12],
    [12, 14, 12, 14, 2, 7, 5, 8],
    [8, 0, 5, 14, 7, 1, 2, 3],
    [3, 14, 9, 10, 10, 9, 14, 3],
    [3, 2, 1, 7, 14, 5, 0, 8],
    [8, 5, 7, 2, 14, 12, 14, 12],
    [12, 2, 2, 0, 1, 4, 13, 3]];
    Alpha = 0b0010;
    # On calcule la matrice L
    L = [[0 for _ in range(8)] for _ in range(8)];
    for i in range(8):
        for j in range(8):
            L[i][j] = powGF16(Alpha, Puissances[i][j])
    # Décalage des lignes à des fins d'optmisation dans le fichier mysterion.c (voir fonction LBox)
    for i in range(8):
        L[i] = L[i][i:]+L[i][:i];
    # On transpose la matrice
    L = t(L) ;
    # On extrait chaque ligne
    for i in range(8):
        L_state = [0x00000000 for _ in range(4)];
        bit = [0 for _ in range(4)];
        for j in range(8):
            tmp_colonne = L[i][j];
            bit[3] = (tmp_colonne >> 0)& 1;
            bit[2] = (tmp_colonne >> 1)& 1;
            bit[1] = (tmp_colonne >> 2)& 1;
            bit[0] = (tmp_colonne >> 3)& 1;
            L_state[0] ^= (((bit[0])^(bit[0] << 8)^(bit[0] << 16)^(bit[0] << 24)) << (7-j));
            L_state[1] ^= (((bit[1])^(bit[1] << 8)^(bit[1] << 16)^(bit[1] << 24)) << (7-j));
            L_state[2] ^= (((bit[2])^(bit[2] << 8)^(bit[2] << 16)^(bit[2] << 24)) << (7-j));
            L_state[3] ^= (((bit[3])^(bit[3] << 8)^(bit[3] << 16)^(bit[3] << 24)) << (7-j));
        print("const m_state colonne_{0} = {{.state = {{ {1}, {2}, {3}, {4}}}}};".format(i, L_state[0], L_state[1], L_state[2], L_state[3]));
# Pour le calcul de la matrice inverse, on s'appuie sur le pivot de Gauss
    L = [[0 for _ in range(8)] for _ in range(8)];
    for i in range(8):
        for j in range(8):
            L[i][j] = powGF16(Alpha, Puissances[i][j]);
    print(L)
    L_Inv = gauss(L);
    print(L_Inv)
    for i in range(8):
        L_Inv[i] = L_Inv[i][i:]+L_Inv[i][:i];
    print(L_Inv)
    L_Inv = t(L_Inv);
    print(L_Inv)

    for i in range(8):
        L_state = [0x00000000 for _ in range(4)];
        bit = [0 for _ in range(4)];
        for j in range(8):
            tmp_colonne = L_Inv[i][j];
            bit[3] = (tmp_colonne >> 0)& 1;
            bit[2] = (tmp_colonne >> 1)& 1;
            bit[1] = (tmp_colonne >> 2)& 1;
            bit[0] = (tmp_colonne >> 3)& 1;
            L_state[0] ^= (((bit[0])^(bit[0] << 8)^(bit[0] << 16)^(bit[0] << 24)) << (7-j));
            L_state[1] ^= (((bit[1])^(bit[1] << 8)^(bit[1] << 16)^(bit[1] << 24)) << (7-j));
            L_state[2] ^= (((bit[2])^(bit[2] << 8)^(bit[2] << 16)^(bit[2] << 24)) << (7-j));
            L_state[3] ^= (((bit[3])^(bit[3] << 8)^(bit[3] << 16)^(bit[3] << 24)) << (7-j));
        print("const m_state colonne_inv_{0} = {{.state = {{ {1}, {2}, {3}, {4}}}}};".format(i, L_state[0], L_state[1], L_state[2], L_state[3]));
