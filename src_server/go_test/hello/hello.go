package hello

import(
	"fmt"
	"net/http"
	"html/template"
	"appengine"
	"appengine/datastore"
);

type User struct{
	Name string
	Role string
};

func init(){
	http.HandleFunc("/", handler1);
	http.HandleFunc("/hoge", handler2);
	http.HandleFunc("/fuga", handler3);
	http.HandleFunc("/myon", handler4);
	http.HandleFunc("/post", handler5);
}

func handler1(w http.ResponseWriter, r *http.Request){
	fmt.Fprint(w, "Hello world");
}

func handler2(w http.ResponseWriter, r *http.Request){
	data := map[string]interface{}{
		"Name": "home",
	};
	render("views/home.html", w, data);
}

func handler3(w http.ResponseWriter, r *http.Request){
	context := appengine.NewContext(r);
	el := User{
		Name: "fuhahaman",
		Role: "dog",
	};
	key, err := datastore.Put(context, datastore.NewIncompleteKey(context, "users", nil), &el);
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError);
		return;
	}
	fmt.Fprint(w, "save dog", key);
}

func handler4(w http.ResponseWriter, r *http.Request){
	context := appengine.NewContext(r);
	q := datastore.NewQuery("users").Limit(10);
	users := make(map[string]User, 0);
	for t := q.Run(context); ; {
		var user User;
		key, err := t.Next(&user)
		if err == datastore.Done {
			break;
		}
		if err != nil {
			http.Error(w, err.Error(), 500)
			return;
		}
		users[key.String()] = user;
	}
	data := map[string]interface{}{
		"Name":  "users/index",
		"Users": users,
	}
	render("views/test.html", w, data);
}

func handler5(w http.ResponseWriter, r *http.Request){
	fmt.Fprint(w, "method: ", r.Method);
	if r.Method == "POST" {
		fmt.Fprint(w, "username:", r.Body);
		fmt.Fprint(w, "password:", r.FormValue("data"));
	}
	
	/*
	import "io/ioutil"
	import "encoding/json"
	// json獲得
	var f interface{}
	w.WriteHeader(200)
	w.Header().Set("Content-Type", "application/json; charset=utf8")
	body, _ := ioutil.ReadAll(r.Body)
	json.Unmarshal(body, &f)
	m := f.(map[string]interface{})
	w.Write([]byte(m["foo"].(string)))
	// クエリ操作メモ
	http://knightso.hateblo.jp/entry/2014/05/26/103458
	*/
}

func render(v string, w http.ResponseWriter, data map[string]interface{}){
	tmpl := template.Must(template.ParseFiles("views/layout.html", v));
	tmpl.Execute(w, data);
}

