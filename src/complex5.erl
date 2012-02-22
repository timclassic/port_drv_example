-module(complex5).
-export([start/1, stop/0, init/1]).
-export([foo/1, bar/1, doob/0]).

start(SharedLib) ->
    PrivDir = code:lib_dir(port_drv_example, priv),
    case erl_ddll:load_driver(PrivDir, SharedLib) of
	ok -> ok;
	{error, already_loaded} -> ok;
	Other -> exit(Other)
    end,
    spawn(?MODULE, init, [SharedLib]).

init(SharedLib) ->
    register(complex, self()),
    Port = open_port({spawn, SharedLib}, [{packet, 2}, binary]),
    loop(Port).

stop() ->
    complex ! stop.

foo(X) ->
    call_port({foo, X}).
bar(Y) ->
    call_port({bar, Y}).
doob() ->
    call_port({doob}).

call_port(Msg) ->
    complex ! {call, self(), Msg},
    receive
	{complex, Result} ->
	    Result
    end.

loop(Port) ->
    receive
	{call, Caller, Msg} ->
	    Port ! {self(), {command, term_to_binary(Msg)}},
	    receive
		{Port, {data, Data}} ->
		    Caller ! {complex, binary_to_term(Data)}
	    end,
	    loop(Port);
	stop ->
	    Port ! {self(), close},
	    receive
		{Port, closed} ->
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("~p ~n", [Reason]),
	    exit(port_terminated)
    end.
