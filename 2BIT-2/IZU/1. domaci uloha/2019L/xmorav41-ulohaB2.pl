% ulohaB2([obj(1,10),obj(2,3),obj(1,8),obj(3,2),obj(5,3)], [obj(2,3),obj(3,2),obj(5,3)]).

% pozrie LOUT v L
exist(L, [obj(X,Y)|TAIL]) :-
   member(obj(X,Y), L),
   exist(L, TAIL).
exist(_,[]).

same([obj(X,Y) | TAIL], LOUT, TMPARR):-
   member(obj(X,_), TAIL),\+member(obj(X,Y), LOUT),append([obj(X,Y)], TMPARR, NEWARR),same(TAIL, LOUT, NEWARR); % Duplicitny obj v L
   member(obj(X,_),TMPARR),\+member(obj(X,Y), LOUT),append([obj(X,Y)], TMPARR, NEWARR),same(TAIL, LOUT, NEWARR); % Duplicitny obj v L
   \+member(obj(X,Y), TAIL),\+member(obj(X,Y), TMPARR),member(obj(X,Y), LOUT),append([obj(X,Y)], TMPARR, NEWARR),same(TAIL, LOUT, NEWARR) % Unikatny obj
   .
same([],_,_).

ulohaB2(LIST, LOUT) :-
   exist(LIST,LOUT),
   same(LIST,LOUT,[])
   .
ulohaB2(_,[]).
