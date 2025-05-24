#ifndef _graphs_h
#define _graphs_h 1
// keep one hour in memory 
#define graph_line_hold_secs 3600
#define graph_set_hold_notes 500

class graph_line {
    public:
    PACKFILE *pf;
    char *name;
    int section_code;
    int color;
    int data[graph_line_hold_secs];
    int valid_secs, on_sec, min, max, back_secs, display;
    int display_index;
    graph_line () {
       pf = NULL;
       name = NULL;
       section_code = 0;
       color = 0;
       valid_secs = on_sec = min = max = back_secs = 0;
       display = 1;
       display_index = 0;           
    }
    int correct_color_by_section(int sc, int col);
    void put_header(PACKFILE *set_pf, char *sname, int ssection_code);
    void load_header(PACKFILE *set_pf, int version = 1);
    void append_data(int dataitm);
    void pf_append_data();
    int get_sec_ago(int delta_t);
    int set_minmax(int delta_t, int count);
};

class graph_set {	
    public:
    PACKFILE *pf;
    BITMAP *screenshot;
    graph_line *graph;
    int version, num_graphs;
    int x, y, w, h;
    int tracker_line, disp_seconds, right_hand;
    void *note[graph_set_hold_notes];
    int valid_notes, on_note;
	int time;
	int load_last_ofs;    
    graph_set() {
       pf = NULL; graph = NULL; version = 1; tracker_line = num_graphs = 0;
       disp_seconds = 300; right_hand = 0; memset(note, 0, sizeof(void *) * graph_set_hold_notes);    
       valid_notes = on_note = time = 0; 
       screenshot = NULL;    
       load_last_ofs = 0;
    }
    void kill() {
       if (graph) delete[] graph; 
       graph = NULL; num_graphs = 0; if (pf) pack_fclose(pf); pf = NULL; 
       tracker_line = num_graphs = 0;  disp_seconds = 300; right_hand = 0;   
       for (int i = 0; i < valid_notes; i++) { free(note[i]); note[i] = NULL; }
       valid_notes = on_note = time = 0;
       if (screenshot) destroy_bitmap(screenshot);
       screenshot = NULL;
       load_last_ofs = 0;     
    }
    ~graph_set() { kill(); }
    void set_draw_zone(int sx, int sy, int sw, int sh);
    void draw(BITMAP *out, int blackout = 1);
    void display_only_user();
    void display_only_alignment();
    void display_all();
    int handle_input(int how_much = 0); // 0 = pause  1 = game   2 = viewer
    int build_new_set();
    void feed_graphs(int money, int fame, int tourists, int ticket_price, int sweetness, int gen_sp, int penalty);
    void notestr_add(char style, char *notestr);    
    int load(char *fn); 
    int load_last(int ith = 0);
    void pf_load_note(char style);
    char *get_notestr_from_sec_ago(int delta_t, int guess_ix, char &style, int &index);
};

extern graph_set *graphs;
#endif
