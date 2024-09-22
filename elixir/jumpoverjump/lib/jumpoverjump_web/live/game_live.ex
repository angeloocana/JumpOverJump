defmodule JumpoverjumpWeb.GameLive do
  use JumpoverjumpWeb, :live_view

  def mount(_params, _session, socket) do
    {:ok, assign(socket, turn: :white)}
  end

  def render(assigns) do
    ~H"""
    <div>
      <h1>Jumpoverjump</h1>

      <%= @turn %>
      <.button phx-click="move">Move</.button>
    </div>
    """
  end

  def handle_event("move", _params, socket) do
    new_turn = if socket.assigns.turn == :white, do: :black, else: :white
    {:noreply, assign(socket, turn: new_turn)}
  end
end
