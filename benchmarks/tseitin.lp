assign(X, Y, 1) | assign(X, Y, 0) :- edge(X, Y).
sum(V, S) :- vertex(V), X = #sum{ A : assign(_, V, A); A : assign(V, _, A) }, S = X - X / 2 * 2.
:- sum(V, S), capacity(V, C), S != C.
