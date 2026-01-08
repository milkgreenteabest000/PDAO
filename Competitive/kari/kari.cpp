#include <iostream>
#include <vector>
#include <queue>
using namespace std;

#define ll long long
#define INF 1e15


int n, m, S, T, A, B;

struct Road{
    int u, v;
    ll cap;
};

vector<int> family;      // 1 = mixing, 0 = non-mixing
vector<Road> roads;

struct Dinic{
    struct Edge{
        int to, rev;
        ll cap;
    };

    int N;
    vector<vector<Edge>> G;
    vector<int> level, it;

    Dinic(int n = 0) { init(n); }

    void init(int n){
        N = n;
        G.assign(N, {});
    }

    void addEdge(int u, int v, ll cap){
        Edge a{v, (int)G[v].size(), cap};
        Edge b{u, (int)G[u].size(), 0};
        G[u].push_back(a);
        G[v].push_back(b);
    }

    bool bfs(int s, int t){
        level.assign(N, -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);

        while(!q.empty()){
            int u = q.front(); q.pop();
            for(auto &e : G[u]){
                if(e.cap > 0 && level[e.to] == -1){
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] != -1;
    }

    ll dfs(int u, int t, ll f){
        if(u == t) return f;

        for(int &i = it[u]; i < (int)G[u].size(); i++){
            auto &e = G[u][i];
            if(e.cap > 0 && level[e.to] == level[u] + 1){
                ll pushed = dfs(e.to, t, min(f, e.cap));
                if (pushed) {
                    e.cap -= pushed;
                    G[e.to][e.rev].cap += pushed;
                    return pushed;
                }
            }
        }
        return 0;
    }

    ll maxFlow(int s, int t){
        ll flow = 0;
        while(bfs(s, t)){
            it.assign(N, 0);
            while (true) {
                ll pushed = dfs(s, t, INF);
                if (!pushed) break;
                flow += pushed;
            }
        }
        return flow;
    }
};

struct Indexer {
    int baseU, baseM, baseR;
    int NeedU, NeedM;
    int SS, TT;

    Indexer(int n){
        baseU = 0;
        baseM = baseU + n;
        baseR = baseM + n;

        NeedU = baseR + n;
        NeedM = NeedU + 1;

        SS = NeedM + 1;
        TT = SS + 1;
    }

    int vU(int v) { return baseU + (v - 1); }
    int vM(int v) { return baseM + (v - 1); }
    int vR(int v) { return baseR + (v - 1); }

    int total() { return TT + 1; }
};

bool feasible(ll supply){
    Indexer id(n);
    Dinic din(id.total());

    int SS = id.SS;
    int TT = id.TT;

    // Entrance supply
    din.addEdge(SS, id.vU(S), supply);

    // Each house must receive exactly one valid bowl
    for(int v = 1; v <= n; v++){
        if(family[v] == 0){               // non-mixing
            din.addEdge(id.vU(v), id.vR(v), 1);
        }
        else{                            // mixing
            din.addEdge(id.vM(v), id.vR(v), 1);
        }
        din.addEdge(id.vR(v), TT, 1);
    }

    // Transfer rules
    auto addTransfer = [&](int u, int v, ll cap){
        // From unmixed at u
        din.addEdge(id.vU(u), family[v] ? id.vM(v) : id.vU(v), cap);

        // From mixed at u
        if(family[v]){
            din.addEdge(id.vM(u), id.vM(v), cap);
        }
    };

    for(auto &e : roads){
        addTransfer(e.u, e.v, e.cap);
        addTransfer(e.v, e.u, e.cap);
    }

    // Export station demand
    din.addEdge(id.vU(T), id.NeedU, B);
    din.addEdge(id.vM(T), id.NeedM, A);

    din.addEdge(id.NeedU, TT, B);
    din.addEdge(id.NeedM, TT, A);

    ll need = (ll)n + A + B;
    ll flow = din.maxFlow(SS, TT);

    return flow >= need;
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> m >> S >> T >> A >> B;

    family.resize(n + 1);
    for(int i = 1; i <= n; i++) cin >> family[i];

    roads.reserve(m);
    for(int i = 0; i < m; i++){
        int u, v;
        ll c;
        cin >> u >> v >> c;
        roads.push_back({u, v, c});
    }

    ll l = n + A + B;
    ll r = 1;
    while (!feasible(r)) r *= 2;

    while (l < r) {
        ll mid = (l + r) >> 1;
        if (feasible(mid)) r = mid;
        else l = mid + 1;
    }

    cout << l << endl;
    return 0;
}
