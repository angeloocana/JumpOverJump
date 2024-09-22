defmodule JumpoverjumpWeb.GameLive do
  use JumpoverjumpWeb, :live_view

  def mount(_params, _session, socket) do
    {:ok, assign(socket, turn: :white, last_move: "", form: to_form(%{from: "", to: ""})) }
  end

  def handle_event("move", %{"from" => from, "to" => to}, socket) do
    new_turn = if socket.assigns.turn == :white, do: :black, else: :white
    {:noreply, assign(socket, turn: new_turn, last_move: from <> " " <> to)}
  end
end
