#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <set>
#include <map>
#include <sstream>
#include <initializer_list>
#include <sys/time.h>

#include "cmdline.h"
#include "bakkjson.hpp"
#include "socket.h"

#define rep(i, n) for(int i = 0; i < (n); i++)
#define all(v) (v).begin(), (v).end()
#define EACH_PLAYER(i) for(int i = 0; i < 4; i++)
#define B(e) {#e,e}

using namespace std;

template <class T> bool compare_with_list(T o1, T o2, vector<T> lst) {
  find(all(lst), o1) < find(all(lst), o2);
}

template <class It, class E> int index(It it1, It it2, E e) {
  return find(it1, it2, e) - it1;
}

typedef json::value::object object;

namespace pai {

  typedef int t;
  enum type {
    MANZU, PINZU, SOZU, JIHAI
  };

  array<string, 38> str = {
    "1m","2m","3m","4m","5m","6m","7m","8m","9m",
    "1p","2p","3p","4p","5p","6p","7p","8p","9p",
    "1s","2s","3s","4s","5s","6s","7s","8s","9s",
    "E","S","W","N","P","F","C","5mr","5pr","5sr","?"
  };

  type get_type(t pai) {
    if(pai == 34) return MANZU;
    if(pai == 35) return PINZU;
    if(pai == 36) return SOZU;
    return (type)(pai / 10);
  }

  int num(t pai) {
    if(pai == 34 || pai == 35 || pai == 36) return 5;
    int x = pai % 10;
    return x ? x : 5;
  }

  t remove_red(t pai) {
    return type(pai) * 10 + num(pai);
  }

  string to_string(t pai) {
    return str[pai];
  }

  t of_string(string s) {
    return index(all(str), s);
  }
}

namespace kaze {

  enum t {
    TON, NANN, SHA, PEI
  };
  
  array<string, 4> str = {"E","S","W","N"};
  
  string to_string(t kaze) {
    return str[kaze];
  }
}

namespace tehai {
  typedef array<int, 38> t;

  t empty() {
    t ret;
    ret.fill(0);
    return ret;
  }

  t add(t& tehai, pai::t pai) {
    tehai[pai]++;
  }
  bool find(t & tehai, pai::t pai) {
    return tehai[pai] > 0;
  }
  void remove(t& tehai, pai::t pai) {
    if(tehai[pai] <= 0) throw 1;
    tehai[pai]--;
  }

  void remove(t& tehai, t consumed) {
    for(int i = 0; i < 37; i++) {
      tehai[i] -= consumed[i];
    }
  }

  bool contains(t tehai1, t tehai2) {
    for(int i = 0; i < 37; i++) {
      if(tehai1[i] < tehai2[i]) return false;
    }
    return true;
  }

  t added(t tehai, pai::t pai) {
    tehai[pai]++;
    return tehai;
  }
}

namespace naki {
  enum type {
    ANKAN, DAIMINKAN, KAKAN, PON, CHI, HORA, NONE
  };

  vector<string> type_str = {"ankan", "daiminkan", "kakan", "pon", "chi", "hora", "none"};

  type type_of_string(string s) {
    return (type)index(all(type_str), s);
  }

  string type_to_string(type t) {
    return type_str[(int)t];
  }
}

namespace mentsu {
  enum type {
    SHUNTSU, MINKOU, ANKOU, MINKAN, ANKAN
  };

  struct t {
    type mentsu_type;
    tehai::t pais;
    t(type mentsu_type, tehai::t pais){
      this->mentsu_type = mentsu_type;
      this->pais = pais;
    }
  };

  vector<type> type_naki = { ANKAN, MINKAN, MINKAN, MINKOU, SHUNTSU };

  type type_of_naki_type(naki::type t) {
    return type_naki[(int)t];
  }

}

namespace yaku {
  enum t {
    TENHO, CHIHO, KOKUSHIMUSO, DAISANGEN, SUANKO,
    TSUISO, RYUISO, CHINROTO, DAISUSHI, SHOSUSHI,
    SUKANTSU, CHURENPOTON, DORA, URADORA, AKADORA,
    REACH, IPPATSU, MENZENCHIN_TSUMOHO, TANYAOCHU,
    PINFU, IPEKO, BAKAZE, JIKAZE, RINSHANKAIHO,
    CHANKAN, HAITEIRAOYUE, HOTEIRAOYUI, SANSHOKUDOJUN, IKKITSUKAN,
    HONCHANTAIYAO, CHITOITSU, TOITOIHO, SANANKO, HONROTO,
    SANSHOKUDOKO, SANKANTSU, SHOSANGEN, DOUBLE_REACH, HONISO,
    JUNCHANTAIYAO, RYANPEKO, CHINISO
  };
}

int shanten(tehai::t tehai) {
  return 0;
}

vector<yaku::t> list_yaku_ron(tehai::t tehai, vector<mentsu::t> open, pai::t pai) {
  return vector<yaku::t>();
}

vector<yaku::t> list_yaku_tsumo(tehai::t tehai, vector<mentsu::t> open, pai::t pai) {
  return vector<yaku::t>();
}

bool yaku_ari(vector<yaku::t> yakus) {
  for(yaku::t y : yakus) {
    if(y != yaku::DORA || y != yaku::URADORA || y != yaku::AKADORA || y != yaku::IPPATSU) {
      return true;
    }
  }
  return false;
}

json::value::array encode_tehai(tehai::t tehai) {
  json::value::array ret;
  for(int i = 0; i < (int)tehai.size(); i++) {
    rep(j, tehai[i]) ret.push_back(pai::to_string(i));
  }
  return ret;
}

tehai::t decode_tehai(json::value::array tehai_json) {
  tehai::t ret;
  for(json::value v : tehai_json) {
    tehai::add(ret, pai::of_string(v));
  }
  return ret;
}

object hello(string protocol, int protocol_version) {
  return object({{"type","hello"},B(protocol),B(protocol_version)});
}

object start_game(int id, array<string, 4> _names) {
  json::value::array names(all(_names));
  return object({{"type","start_game"},B(id), B(names)});
}

object start_kyoku(kaze::t _bakaze, int kyoku, int honba, int kyotaku, int oya, pai::t _dora_marker, array<tehai::t, 4> _tehais) {
  string bakaze = kaze::to_string(_bakaze);
  string dora_marker = pai::to_string(_dora_marker);
  json::value::array tehais(4);
  EACH_PLAYER(i) {
    tehais[i] = encode_tehai(_tehais[i]);
  }
  return object({{"type","start_kyoku"},B(bakaze),B(kyoku),B(honba),B(kyotaku),B(oya),B(dora_marker),B(tehais)});
}

object tsumo(int actor, pai::t _pai) {
  string pai = pai::to_string(_pai);
  return object({{"type","tsumo"},B(actor),B(pai)});
}

object dahai(int actor, pai::t _pai, bool tsumogiri) {
  string pai = pai::to_string(_pai);
  return object({{"type","dahai"},B(actor),B(pai),B(tsumogiri)});
}

object reach(int actor) {
  return object({{"type","reach"},B(actor)});
}

object reach_accepted(int actor, array<int, 4> _deltas, array<int, 4> _scores) {
  json::value::array deltas(all(_deltas));
  json::value::array scores(all(_scores));
  return object({{"type","reach_accepted"},B(actor),B(deltas),B(scores)});
}

object chi_pon_daiminkan(naki::type _type, int actor, int target, pai::t _pai, tehai::t _consumed) {
  string type = naki::type_to_string(_type);
  string pai = pai::to_string(_pai);
  json::value::array consumed = encode_tehai(_consumed);
  return object({B(type),B(actor),B(target),B(pai),B(consumed)});
}

object ankan(int actor, tehai::t _pais) {
  json::value::array pais = encode_tehai(_pais);
  return object({{"type","ankan"}, B(actor), B(pais)});
}

object end_kyoku() {
  return object({{"type","end_kyoku"}});
}

object end_game() {
  return object({{"type","end_game"}});
}

object error(string message) {
  return object({{"type","error"},B(message)});
}

array<gimite::socket_stream*, 4> sock;

void send(int player, json::value json) {
  *sock[player] << json << endl;
  cerr << "-> player " << player << " " << json << endl;
}

void send_each_player(object json) {
  EACH_PLAYER(i) {
    send(i, json);
  }
}

double recv_time = 0;
timeval start_tv;
timeval end_tv;

double to_sec(timeval tv) { return tv.tv_sec + tv.tv_usec * 1e-6; }

object recv(int player) {
  string line;
  gettimeofday(&start_tv, NULL);
  getline(*sock[player], line);
  gettimeofday(&end_tv, NULL);
  recv_time += to_sec(end_tv) - to_sec(start_tv);
  cerr << "<- player " << player << " " << line << endl;
  json::value json = json::parse(line);
  return json;
}

enum mjai_error {
  MISPLAYED
};

void misplayed(int player, string message) {
  stringstream ss;
  ss << "player " << player << " invalid command: " << message;
  string s = ss.str();
  send_each_player(error(s));
  throw MISPLAYED;
}

void confirm_none(int player) {
  object json = recv(player);
  if((string)json["type"] != "none") {
    misplayed(player, "Should return none.");
  }
}

vector<pai::t> create_yama() {
  vector<pai::t> yama;
  for(int i = 0; i < 34; i++) {
    int n = (i % 9 == 4) ? 3 : 4;
    rep(j, n) yama.push_back(i);
  }
  yama.push_back(34);
  yama.push_back(35);
  yama.push_back(36);
  random_shuffle(all(yama));
  return yama;
}

pai::t tsumo_1(vector<pai::t> & yama) {
  pai::t ret = yama.back();
  yama.pop_back();
  return ret;
}

bool compare_naki(object o1, object o2) {
  string t1 = o1["type"];
  string t2 = o2["type"];
  if(t1 != t2) {
    return compare_with_list(t1, t2, {"daiminkan", "pon", "chi", "none"});
  }
  if(t1 == "none") return true;
  return (int)o1["actor"] < (int)o2["actor"];
}

int kyoku = 1;
int honba = 0;
int kyotaku = 0;

array<int, 4> scores;
int actor;

vector<pai::t> yama;
vector<pai::t> dora_markers;
vector<pai::t> uradora_markers;
array<tehai::t, 4> tehais;
pai::t tsumo_pai;
array<vector<pai::t>, 4> kawas;
array<vector<mentsu::t>, 4> opens;
array<bool, 4> reaches;

kaze::t bakaze() { return (kaze::t)((kyoku - 1) / 4); }
int oya() { return (kyoku - 1) % 4; }

void greetings(string room) {
  send_each_player(hello("mjsonp", 3));
  
  array<string, 4> names;

  EACH_PLAYER(i) {
    object json = recv(i);
    if((string)json["type"] != "join") {
      misplayed(i, "Players must return join in response to hello.");
      return;
    }
    if(!json.count("name")) {
      misplayed(i, "The field `name` is missing.");
      return;
    }
    if(!json.count("room")) {
      misplayed(i, "The field `room` is missing.");
      return;
    }
    if((string)json["room"] != room) {
      misplayed(i, "No such room.");
    }
    names[i] = (string)json["name"];
  }
  
  EACH_PLAYER(i) {
    send(i, start_game(i, names));
  }

  EACH_PLAYER(i) {
    confirm_none(i);
  }

  EACH_PLAYER(i) {
    scores[i] = 25000;
  }

}

void haipai() {

  yama        = create_yama();
  rep(i, 7) dora_markers.push_back(tsumo_1(yama));
  rep(i, 7) uradora_markers.push_back(tsumo_1(yama));
  tsumo_pai   = -1;
  tehais.fill(tehai::empty());
  kawas.fill(vector<pai::t>());
  reaches.fill(false);
    
  EACH_PLAYER(i) {
    for(int j = 0; j < 13; j++) {
      tehai::add(tehais[i], tsumo_1(yama));
    }
  }
  
  tehai::t unknown_tehai = tehai::empty();
  rep(i, 13) tehai::add(unknown_tehai, 37);
  
  EACH_PLAYER(i) {
    array<tehai::t, 4> tehais_concealed;
    EACH_PLAYER(j) {
      if(j == i) tehais_concealed[j] = tehais[j];
      else tehais_concealed[j] = unknown_tehai;
    }
    send(i, start_kyoku(bakaze(), kyoku, honba, kyotaku, oya(), dora_markers[0], tehais_concealed));
  }

  EACH_PLAYER(i) {
    confirm_none(i);
  }

}

enum hora_type {
  TSUMO, RON
};

/* If t == RON, then you must add agari_pai to hora_actor's tehai before you call this function */
void on_hora(hora_type t, int hora_actor) {
  kyoku++;
  return;
  tehai::t tehai = tehais[actor];
  
  if(shanten(tehais[actor]) != -1) {
    misplayed(actor, "You cannot tsumo unless shanten of the tehai is -1.");
  }
  // TODO furiten check
  vector<yaku::t> yakus = list_yaku_tsumo(tehais[actor], opens[actor], tsumo_pai);
  if(!yaku_ari(yakus)) {
    misplayed(actor, "You cannot hora unless the tehai is yakuari.");
  }
  // TODO TODO
}

// returns true if the kyoku ends.
bool on_dahai(object json) {
  pai::t dahai_pai = pai::of_string(json["pai"]);
  bool tsumogiri = json["tsumogiri"];

  if(!tehai::find(tehais[actor], dahai_pai)) {
    misplayed(actor, "The selected pai is not in hand");
  }

  if(tsumogiri && dahai_pai != tsumo_pai) {
    misplayed(actor, "Tsumogiri is set to true but it's actually not.");
  }

  kawas[actor].push_back(dahai_pai);
  send_each_player(dahai(actor, dahai_pai, tsumogiri));

  array<object, 4> responses;

  EACH_PLAYER(j) {
    responses[j] = recv(j);
  }

  object naki_json = *max_element(all(responses), compare_naki);
	    
  naki::type naki_type = naki::type_of_string(naki_json["type"]);

  cerr << naki_type << endl;

  if(naki_type == naki::NONE) {
    actor = (actor + 1) % 4;
  }
  else if(naki_type == naki::HORA) {
    int naki_actor = naki_json["actor"];
    tehai::add(tehais[naki_actor], dahai_pai);
    on_hora(TSUMO, naki_actor);
    return true;
  }
  else {
    int naki_actor = naki_json["actor"];
    tehai::t consumed = decode_tehai(naki_json["consumed"]);
    if(!tehai::contains(tehais[naki_actor], consumed)) {
      misplayed(naki_actor, "You must have consumed pais.");
    }
    tehai::remove(tehais[naki_actor], consumed);
    naki::type t = naki::type_of_string(naki_json["type"]);
    mentsu::type mt = mentsu::type_of_naki_type(t);
    opens[naki_actor].push_back(mentsu::t(mt, tehai::added(consumed, dahai_pai)));
    send_each_player(chi_pon_daiminkan(t, naki_actor, actor, dahai_pai, consumed));

    actor = (naki_actor + 1) % 4;

    object json = recv(actor);
    EACH_PLAYER(i) {
      if(i != actor) confirm_none(i);
    }

    if((string)json["type"] != "dahai") {
      misplayed(actor, "Should return dahai after furo.");
    }

    tsumo_pai = -1;
    on_dahai(json);
  }
  return false;
}

void on_ryukyoku() {
  kyoku++;
}

void handle_one_kyoku() {

  haipai();

  actor = oya();
  while(true) {

    if(yama.empty()) {
      on_ryukyoku();
      return;
    }

    tsumo_pai = tsumo_1(yama);
    tehai::add(tehais[actor], tsumo_pai);

    EACH_PLAYER(i) {
      send(i, tsumo(actor, i == actor ? tsumo_pai : 37));
    }

    object json = recv(actor);
    EACH_PLAYER(i) {
      if(i != actor) confirm_none(i);
    }
    // TODO: Field confirmation
    string t = json["type"];
    if(t == "dahai") {
      if(on_dahai(json)) return;
    }
    else if(t == "hora") {
      on_hora(TSUMO, actor);
      return;
    }
    else if(t == "reach") {
      send_each_player(reach(actor));
      object dahai_json = recv(actor);
      EACH_PLAYER(i) {
	if(i != actor) confirm_none(i);
      }

      if((string)dahai_json["type"] != "dahai") {
	misplayed(actor, "You should return dahai after reach.");
      }

      int dahai_actor = actor;

      if(on_dahai(dahai_json)) return;
	  
      array<int, 4> deltas;
      EACH_PLAYER(i) {
	deltas[i] = (i == dahai_actor ? -1000 : 0);
	scores[i] += deltas[i];
      }
      reaches[dahai_actor] = true;
      send_each_player(reach_accepted(dahai_actor, deltas, scores));
      EACH_PLAYER(i) {
	confirm_none(i);
      }
    }
    else if(t == "ankan") {
      tehai::t consumed = decode_tehai(json["consumed"]);
      if(!tehai::contains(tehais[actor], consumed)) {
	misplayed(actor, "You does not have consumed pais when ankan.");
      }
      tehai::remove(tehais[actor], consumed);
      opens[actor].push_back(mentsu::t(mentsu::ANKAN, consumed));
	  
      send_each_player(ankan(actor, consumed));
    }
    else if(t == "kakan") {
      pai::t pai = pai::of_string(json["pai"]);
      tehai::t consumed = decode_tehai(json["consumed"]);
      
      bool found = false;
      for(mentsu::t& m : opens[actor]) {
	if(m.pais == consumed) {
	  m.mentsu_type = mentsu::MINKAN;
	  tehai::add(m.pais, pai);
	  found = true;
	  break;
	}
      }

      if(!found) {
	misplayed(actor, "You cannot kakan.");
      }
    }
    else {
      cerr << t << endl;
      misplayed(actor, "Should return dahai, hora, reach, kakan, or ankan");
    }
  }
}

void play(string room) {

  greetings(room);

  while(kyoku < 8) {
    handle_one_kyoku();
    send_each_player(end_kyoku());
    EACH_PLAYER(i) {
      confirm_none(i);
    }
  }

  send_each_player(end_game());
  
}

void run(int port, string room) {
  
  gimite::startup_socket();
  gimite::server_stream_socket server(port);
  
  if(!server) {
    cerr << "failed to bind socket" << endl;
    return;
  }

  try {

    EACH_PLAYER(i) {
      cerr << "Waiting for " << (4 - i) << " more players" << endl;
      sock[i] = new gimite::socket_stream(server.accept());
    }
    
    random_shuffle(all(sock));
    
    play(room);
  }
  catch(exception) {
    gimite::cleanup_socket();
    throw;
  }

  gimite::cleanup_socket();

}

int main(int argc, char **argv) {

  cmdline::parser p;
  p.add<int>("port", 'p', "port to listen", false, 11600);
  p.add<string>("room", 'r', "room to listen", false, "default");
  p.add("help", 'h', "print help");

  if(!p.parse(argc, argv) || p.exist("help")) {
    cerr << p.error_full() << p.usage() << endl;
    return 1;
  }

  run(p.get<int>("port"), p.get<string>("room"));

  cerr << "recv_time = " << recv_time << endl;

  return 0;
}
