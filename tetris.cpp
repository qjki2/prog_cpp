#include "tetris.hpp"

bool isPower(uint32_t n){
	if (n <= 0) return false; 
	while(n%2 == 0){
		n /= 2; 
	} return n == 1; 
}

    piece::piece(){
    	m_side = 0; 
    	m_color = 0; 
    	m_grid = nullptr; 
    }

    piece::piece(uint32_t s, uint8_t c){
    	if ( !isPower(s) || c == 0) throw tetris_exception("s non e power di 2 oppure c non e maggiore di 0");

    	m_side = s; 
    	m_color = c; 
    	m_grid = new bool*[m_side]; 
    	for (uint32_t i = 0; i<m_side; i++){
    		m_grid[i]= new bool[m_side];
    		for (uint32_t j = 0; j<m_side; j++){
    			m_grid[i][j]= false;
    		}
    	} 
    }

	piece::piece(piece const& rhs){
		m_side = rhs.m_side;
		m_color = rhs.m_color;
		m_grid = new bool*[m_side];
		for (uint32_t i = 0; i<m_side; i++){
			m_grid[i] = new bool[m_side];
			for (uint32_t j = 0; j<m_side; j++){
				m_grid[i][j]= rhs.m_grid[i][j];
			}
		}
	}

	piece::piece(piece&& rhs){ 
		*this = std::move(rhs);
	}

	piece::~piece(){
		for (uint32_t i = 0; i< m_side; i++){
			delete[] m_grid[i];
		}
		delete[] m_grid; 
	}

    piece& piece::operator=(piece const& rhs){
    	if(this != &rhs) { 

    		for (uint32_t i = 0; i< m_side; i++){ 
				delete[] m_grid[i];
			}
			delete[] m_grid; 

			m_side = rhs.m_side;
			m_color = rhs.m_color;
			m_grid = new bool*[m_side];

			for (uint32_t i = 0; i<m_side; i++){
				m_grid[i] = new bool[m_side];
				for (uint32_t j = 0; j<m_side; j++){
					m_grid[i][j]= rhs.m_grid[i][j];
				}
			}
    	} 
    	return *this;
    }


    piece& piece::operator=(piece&& rhs){
    	if(this != &rhs){
    		for (uint32_t i = 0; i< m_side; i++){
				delete[] m_grid[i];
			}
			delete[] m_grid; 

			m_side = rhs.m_side; 
			m_color = rhs.m_color; 
			m_grid = rhs.m_grid; 

			rhs.m_side = rhs.m_color = 0;
			rhs.m_grid = nullptr; 
    	}
    	return *this; 
    }

    bool piece::operator==(piece const& rhs) const{
    	if(m_color != rhs.m_color) return false;
    	if (m_side != rhs.m_side) return false; 

    	for (uint32_t i = 0; i< m_side; i++){
    		for (uint32_t j = 0; j<m_side; j++){
    			if(m_grid[i][j] != rhs.m_grid[i][j]) return false; 
    		}
    	}
    	return true; 
    }


    bool piece::operator!=(piece const& rhs) const{
    	return !(*this == rhs);

    }

    bool& piece::operator()(uint32_t i, uint32_t j){
    	if (i > m_side || j > m_side || i<0 || j<0) throw tetris_exception ("i,j out of bonds");

    	return m_grid[i][j];
    }

    bool piece::operator()(uint32_t i, uint32_t j) const{
    	if (i > m_side || j > m_side || i<0 || j<0) throw tetris_exception ("i,j out of bonds");

    	return m_grid[i][j];
    }

    // correggere tutte le empty e le full

    bool piece::empty(uint32_t i, uint32_t j, uint32_t s) const{
    	if (s + i > m_side || s+j > m_side) throw tetris_exception ("s troppo grande");
    	for (uint32_t r = i; r<s+i; r++){
    		for (uint32_t c = j; c < s+j; c++ ){
    			if (m_grid[r][c] == true) return false; 
    		}
    	}
    	return true; 
    }

    bool piece::full(uint32_t i, uint32_t j, uint32_t s) const{ // provo a afre il contrario di empty()
    	if (s + i > m_side || s+j > m_side || s<=0) throw tetris_exception ("s troppo grande");
    	if (!m_grid) return false; 
    	for (uint32_t r = i; r<s+i; r++){
    		for (uint32_t c = j; c < s+j; c++ ){
    			if (m_grid[r][c] == false) return false; 
    		}
    	}
    	return true; 
    }

    bool piece::empty() const{
    	for (uint32_t i = 0; i<m_side; i++){
			for (uint32_t j = 0; j<m_side; j++){
				if(m_grid[i][j] == true ) return false; 
			}
		}
		return true; 
    }

    bool piece::full() const{
    	for (uint32_t i = 0; i<m_side; i++){
			for (uint32_t j = 0; j<m_side; j++){
				if(m_grid[i][j] != true ) return false; 
			}
		}
		return true; 
    }

    void piece::rotate(){
    	bool** n = new bool*[m_side];
    	for (uint32_t i = 0; i < m_side; i++){
    		n[i]= new bool[m_side];
    	}

    	for (uint32_t i = 0; i< m_side; i++){
    		for (uint32_t j =0; j < m_side; j++){
    			n[j][m_side-i-1] = m_grid[i][j];
    		}
    	}
    	for (uint32_t i = 0; i< m_side; i++){
    		delete []m_grid[i];
    	}
    	delete[] m_grid; 

    	m_grid = n; 
    }

    void piece::cut_row(uint32_t i){
    	if (i >= m_side) throw tetris_exception("i fuori");

    	if (i == 0 ){
    		for (uint32_t c = 0; c < m_side; c++){
    		m_grid[0][c]= false; 
    		}
    	}else{
    		for (uint32_t r = i; r > 0; r--){
    			for (uint32_t c = 0; c < m_side; c++){
    				m_grid[r][c]= m_grid[r-1][c];
    			}
    		}
    		for (uint32_t c = 0; c < m_side; c++){
    			m_grid[0][c]= false; 
    		}
    	}
    }

    void piece::print_ascii_art(std::ostream& os) const{}

    uint32_t piece::side() const{
    	return m_side;
    }
    int piece::color() const{
    	return m_color;
    }

    void saltaSpazzi(std::istream& is){ 
    	char c; 
    	is.get(c);
    	if(c == '\n' || c =='\t' || c == ' ') saltaSpazzi(is); 
    	else
    		is.putback(c);
    }

	int abiba(int n){
		if (n<= 0) return 0; 
		else{
			int res = 0;
			while(n>1){
				n/= 2; 
				res++;
			}
			return res; 
		}
	}

    void funzione(std::istream& is, piece& p, int x, int y, int size, int profondita);

    void riempi(piece& p, int x, int y, int size, bool value) {
		for (int i = x; i < x + size; ++i)
			for (int j = y; j < y + size; ++j)
				p(i, j) = value;
	}

	void parentesi_quadra(std::istream& is, piece& p, int x, int y, int size) {
		char c;
		saltaSpazzi(is);
		is.get(c);
		if (c != ']') throw tetris_exception("']' mancante || carattere non valido");
		riempi(p, x, y, size, false);
	}

	void parentesi_tonda(std::istream& is, piece&p, int x, int y, int size, int profondita){ 
		saltaSpazzi(is);

			if (is.peek() == ')') {
				is.get();
				riempi(p, x, y, size, true);
			} else {
				int half = size / 2;
				if (profondita < 0) throw tetris_exception ("side errata(raggiunta prfondita max della ricorsione)");
				funzione(is, p, x, y, half, profondita-1);
				funzione(is, p, x, y + half, half, profondita-1);
				funzione(is, p, x + half, y, half, profondita-1);
				funzione(is, p, x + half, y + half, half, profondita-1);

				saltaSpazzi(is);
				if (is.get() != ')') throw tetris_exception("')' mancante || carattere non valido");
			}
	}

	void funzione(std::istream& is, piece& p, int x, int y, int size, int profondita) {

		if (profondita < 0 ) throw tetris_exception ("side errata(raggiunta prfondita max della ricorsione)");

		saltaSpazzi(is);
		char c = is.get();
		if (c == '[') {
			saltaSpazzi(is);
			parentesi_quadra(is, p, x, y, size);
		} else if (c == '(') {
			saltaSpazzi(is);
			parentesi_tonda(is, p, x, y, size, profondita);
		} else{
			throw tetris_exception("carattere non valido");
		}
	}

	std::istream& operator>>(std::istream& is, piece& p){
		uint32_t side;
		uint32_t color;

		saltaSpazzi(is);

		is >> side; 
		if(is.fail() || is.bad()) throw tetris_exception("operator >> : numero non valido");
		if(!isPower(side)) throw tetris_exception("side non power di 2");

		saltaSpazzi(is);

		is >> color; 
		if (is.fail() || is.bad() ) throw tetris_exception("colore non valido");
		if (color > 255 || color < 1) throw tetris_exception ("colore non valido");

		saltaSpazzi(is);

		p = piece(side, color);
		int profonditaMax = abiba(side); 
		funzione(is, p, 0, 0, side, profonditaMax);

		return is;
	}

	std::string par(const piece& p, uint32_t x, uint32_t y, uint32_t side){
		bool full = true;
		bool empty = true;

		for (uint32_t i = x; i < x + side; i++) {
			for (uint32_t j = y; j < y + side; j++) {
				if (p(i, j)) empty = false;
				else full = false;
			}
		}
		 if (full) return "()"; 
		 if (empty) return "[]";
		 return "nor";
	}

	std::string func(const piece& p, uint32_t x, uint32_t y, uint32_t side) {
		if (side < 1) throw tetris_exception("side minore di 1 ");

		if (par(p, x, y, side) != "nor") return ( par (p, x, y, side) == "()") ? "()" : "[]";

    	uint32_t meta= side / 2;
    	return "(" + func(p, x, y, meta)+ func(p, x, y + meta, meta)+ func(p, x + meta, y, meta)+ func(p, x + meta, y + meta, meta)+ ")";
	}

	std::ostream& operator<<(std::ostream& os, piece const& p){
		uint32_t side = p.side();
		if (side < 0) throw tetris_exception("side non valido");
		if( side == 0) throw tetris_exception("side non puo essere 0");
		uint32_t color = p.color();
		if ( color < 1 || color > 255 ) throw tetris_exception("colore non valido");

		os << side << ' ' << color << ' ';
		os << func(p, 0, 0, side);

    	return os;
	}

tetris::tetris() {
	m_score=m_width=m_height=0;
	m_field=nullptr;
}

tetris::tetris(uint32_t w, uint32_t h, uint32_t s) {
	if (h == 0 || w == 0) throw tetris_exception("dimensione non valida");
	m_width = w; 
	m_score = s; 
	m_height = h; 
	m_field = nullptr; 
}

tetris::tetris(tetris const& rhs) {
	m_score = rhs.m_score;
	m_width = rhs.m_width;
	m_height = rhs.m_height; 
	m_field = nullptr;

	node* pc = rhs.m_field; 
	while(pc){
		tetris_piece tp_c; 
		tp_c.x = pc->tp.x; 
		tp_c.y = pc->tp.y; 
		tp_c.p = pc->tp.p; 

		node* new_node = new node{tp_c, nullptr};

    	if (!m_field) {
        	m_field = new_node;
    	} else {
        	node* curr = m_field;
        	while (curr->next) {
            	curr = curr->next;
        	}
        	curr->next = new_node;
    	}
		pc = pc->next; 
	}
}

tetris::tetris(tetris&& rhs) {
	*this = std::move(rhs);
}

tetris::~tetris() {
	node* pc = m_field; 
	while(pc){
		node* del = pc; 
		pc = pc->next; 
		delete del; 
	}
}

tetris& tetris::operator=(tetris const& rhs) {
	if (this != &rhs){

		while(m_field){
			node* del = m_field; 
			m_field = m_field->next; 
			delete del; 
		}

		m_score = rhs.m_score;
	    m_width = rhs.m_width;
	    m_height = rhs.m_height;
	    m_field = nullptr; 

	    node* pc = rhs.m_field;

	    while(pc){
			tetris_piece tp_c; 
			tp_c.x = pc->tp.x; 
			tp_c.y = pc->tp.y; 
			tp_c.p = pc->tp.p; 

			node* new_node = new node{tp_c, nullptr};

	    	if (!m_field) {
	        	m_field = new_node;
	    	} else {
	        	node* curr = m_field;
	        	while (curr->next) {
	            	curr = curr->next;
	        	}
	        	curr->next = new_node;
	    	}
			pc = pc->next; 
		}
	}
    return *this;
}

tetris& tetris::operator=(tetris&& rhs) {
	if (this != &rhs){

		while(m_field){
			node* del = m_field; 
			m_field = m_field->next;
			delete del; 
		}
		m_score = rhs.m_score;
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        m_field = rhs.m_field;

        rhs.m_field= nullptr; 
        rhs.m_score = rhs.m_width = rhs.m_height = 0;

	}
    return *this;
}

bool tetris::operator==(tetris const& rhs) const {
	if (m_score!=rhs.m_score || m_width != rhs.m_width || m_height!= rhs.m_height) return false;

    node* pc = m_field; 
    node* rc = rhs.m_field; 
    while(pc && rc){
    	if(pc->tp.x != rc->tp.x || pc->tp.p != rc->tp.p || pc->tp.y != rc->tp.y) return false; 
    	pc = pc->next;
    	rc= rc->next;
    }

    return rc == nullptr && pc == nullptr;
}

bool tetris::operator!=(tetris const& rhs) const {
    return !(*this == rhs);
}

void tetris::insert(piece const& p, int x){

    auto del = [this](int n, int m) {
		node* d =nullptr; 
        node* g = nullptr;
        node* node =m_field;

    	if (m_field->tp.x == n && m_field->tp.y == m ){
        	d = m_field; 
        	m_field = m_field->next; 
        	delete d;
    	}else {
        	while ( node->next && ( node->next->tp.x != n || node->next->tp.y != m )){ 
           	 	node =node->next;
           	}
        	if (node->next ){	
	            g =node->next; 
	            node->next =node->next->next;
	            delete g;
       	 	} 
    	}
    };

    auto pToCX = [](int i, int x){
	    return x + i;
	};

	auto pToCY = [](int j, int y, int side){
	    return y - ((side - 1) - j);
	};


	auto CxToPj = [] (int c, int x){ 
		return c - x; 
	};

	auto CyToPi = [](int r, int y, int side){ 
		return r - y + (side - 1);
	};	

	auto cont = [&](node* pc, int x, int y) ->bool{
		node* prev = nullptr;
		node* p = m_field;
		bool ris = false; 
		while( p && p!= pc ){
			prev = p; 
			p = p->next;
		}
			if (prev)
				prev->next = pc ->next; 
			else
				m_field = pc->next; 

			ris = containment(pc->tp.p, x, y);

			if (prev){
				pc->next = prev->next; 
				prev->next = pc; 
			}else{
				pc->next = m_field; 
				m_field = pc; 
			} 
		return ris; 
	};

    int y = m_height-1 + p.side()-1;  
    bool inserito = false; 
    while(y >= 0 && !inserito) {
    	if (containment(p, x, y)){
    		add(p, x, y);
    		inserito = true; 
    	}
    	else 
    		y--; 
    }

    if (y<0) throw tetris_exception("GAME OVER");


    bool modificato = true;  
    while(modificato){

    	modificato = false; 
	    node* pc = m_field; 
    	for (int r = 0; r < m_height; r++){

    		int count = m_width;
    		bool fuori = false; 
    		for(int c = 0; c < m_width && !fuori; c++ ){ 
    			pc = m_field; 
    			while(pc){
    				int i = CyToPi(r, pc->tp.y, pc->tp.p.side());
    				int j = CxToPj(c, pc->tp.x);
    				if (i >= pc->tp.p.side() || j >= pc->tp.p.side() || i<0 || j<0) fuori = true;
    				if (!fuori && pc->tp.p(i, j) == true){
    					count--;
    				}
    				fuori = false; 
    				pc = pc->next;
    			}
    		}
    		if (count == 0){
    			pc = m_field; 
    			bool cutRow = false;
    			while(pc){
    				 
    				int i = CyToPi(r, pc->tp.y, pc->tp.p.side()); 
    				if (!(i >= pc->tp.p.side() ||  i<0 )){
    					pc->tp.p.cut_row(i);
    					cutRow = true; 
    				}
    				pc = pc->next; 
    			}
    			if (cutRow){
    				m_score += m_width;
    				}
    				 pc = m_field;
						while(pc){
							bool inserito = false; 
							int y = m_height-1 + pc->tp.p.side()-1; 
							while(y >= 0 && !inserito) {
						    	if (cont(pc, pc->tp.x, y)){
						    		pc->tp.y = y;
						    		inserito = true; 
						    	}
						    	else 
						    		y--; 
						    }
						    	pc = pc->next; 
						}
				modificato = true;
    		}
    		 
    	}
    }

   	node* pc = m_field; 
    while(pc){
    	if(pc->tp.p.empty()){
    		del(pc->tp.x, pc->tp.y);
    		pc = m_field; 
    	}else
    		pc = pc->next; 
    }

}



void tetris::add(piece const& p, int x, int y) {
	if (!containment(p, x, y)) throw tetris_exception("piece non puo essere aggiunto");

	tetris_piece t;
	t.p = p; 
	t.x = x; 
	t.y = y; 

	node* n = new node{t, m_field};
	m_field = n; 
}

bool tetris::containment(piece const& p, int x, int y) const {

	auto pToCX = [](int i, int x){
	    return x + i;
	};

	auto pToCY = [](int j, int y, int side){
	    return y - ((side - 1) - j);
	};

	auto fuori = [&](int pX, int pY) ->bool{

	    if (pX < 0 || pX >= m_width ||pY < 0 || pY >= m_height ){
	        return true;
	    }
	    return false;
	};

	auto collisione= [&](int i, int j, int x, int y, node* pc, int side)-> bool {
	    int px =pc->tp.x;
	    int py = pc->tp.y;
	    int pside = pc->tp.p.side();
	    for (int j2 =0; j2 <pc->tp.p.side(); ++j2) {
	        for (int i2= 0;i2 < pc->tp.p.side(); ++i2) {
	            if (pc->tp.p(i2, j2)) {
	            	int pcX = pToCX(j2, px);
	            	int thX = pToCX(i, x);
	            	int pcY = pToCY(i2, py, pside);
	            	int thY = pToCY(j, y, side);
	                if (pcX == thX && pcY == thY) {
	                    return true;
	                }
	            }
	        }
	    }
	    return false;
	};

	if(y < 0) throw tetris_exception("y non puo essere < 0");

	for (int j = 0; j < p.side(); ++j){ 
		for (int i = 0; i < p.side(); ++i){
			if(p(i,j) == true){
				int pX = pToCX(j, x);
				int pY = pToCY(i, y, p.side());

				if (fuori( pX, pY)) return false;

				node* pc = m_field; 
				while(pc){
					if (collisione(j, i, x, y, pc, p.side())){
						return false;
					}
					pc = pc->next;
				}
			}
		}
}
    return true;
}

void tetris::print_ascii_art(std::ostream& os) const {}


tetris::iterator::iterator(node* ptr) : m_ptr(ptr) {}

tetris::iterator::reference tetris::iterator::operator*() {
    return m_ptr->tp;
}

tetris::iterator::pointer tetris::iterator::operator->() {
    return &(m_ptr->tp);
}

tetris::iterator& tetris::iterator::operator++() {
	if (m_ptr) m_ptr = m_ptr->next; 
    return *this;
}

tetris::iterator tetris::iterator::operator++(int /*dummy*/) {
	iterator tmp = *this;
	++(*this);
    return tmp;
}

bool tetris::iterator::operator==(iterator const& rhs) const {
    return m_ptr == rhs.m_ptr;
}

bool tetris::iterator::operator!=(iterator const& rhs) const {
    return !(*this == rhs);
}


tetris::const_iterator::const_iterator(node const* ptr) : m_ptr(ptr) {
}

tetris::const_iterator::reference tetris::const_iterator::operator*() const {
    return m_ptr->tp;
}

tetris::const_iterator::pointer tetris::const_iterator::operator->() const {
    return &(m_ptr->tp);
}

tetris::const_iterator& tetris::const_iterator::operator++() {
	if (m_ptr)
		m_ptr = m_ptr->next; 
    return *this;
}

tetris::const_iterator tetris::const_iterator::operator++(int /*dummy*/) {
	const_iterator tmp = *this;
	++(*this);
    return tmp;
}

bool tetris::const_iterator::operator==(const_iterator const& rhs) const {
    return m_ptr == rhs.m_ptr;
}

bool tetris::const_iterator::operator!=(const_iterator const& rhs) const {
    return !(*this == rhs);
}


tetris::iterator tetris::begin() {
    return iterator(m_field);
}

tetris::iterator tetris::end() {
    return iterator(nullptr);
}

tetris::const_iterator tetris::begin() const {
    return const_iterator(m_field);
}

tetris::const_iterator tetris::end() const {
    return const_iterator(nullptr);
}


uint32_t tetris::score() const {
    return m_score;
}

uint32_t tetris::width() const {
    return m_width;
}

uint32_t tetris::height() const {
    return m_height;
}

std::ostream& operator<<(std::ostream& os, tetris const& t) { 
	os << t.score() << " " << t.width() << " " << t.height() <<std::endl;
	tetris::const_iterator it = t.begin();
    while (it != t.end()) {
        os << it->p << " "<< it->x<< " "<< it->y<< std::endl;
        ++it;  
    }

    return os;

}

void funzione2(std::istream& is, tetris& t) {
    while (is) {
    	saltaSpazzi(is);

        piece p(1,1);
        int x, y;

        if (!(is >> p)) throw tetris_exception("piece non valido");
        if (!(is >> x)) throw tetris_exception("numero x non valido");
        if (!(is >> y)) throw tetris_exception("numero y non valido");

        t.add(p, x, y); 

       	saltaSpazzi(is);
    }
}

std::istream& operator>>(std::istream& is, tetris& t) {
    saltaSpazzi(is);
    uint32_t score = 0;
    is >> score;
    if (is.fail() || is.bad()) throw tetris_exception("score non valido");

    saltaSpazzi(is);

    uint32_t width = 0;
    is >> width;
    if (is.fail() || is.bad()) throw tetris_exception("width non valido");
    saltaSpazzi(is);

    uint32_t height = 0;
    is >> height;
    if (is.fail() || is.bad()) throw tetris_exception("height non valido");
    saltaSpazzi(is);
    t = tetris(width, height, score);

    funzione2(is, t);

    return is;
}







