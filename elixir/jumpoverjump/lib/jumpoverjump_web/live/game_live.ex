defmodule JumpoverjumpWeb.GameLive do
  use JumpoverjumpWeb, :live_view

  def mount(_params, _session, socket) do
    {:ok, assign(socket, turn: :white, last_move: "", form: to_form(%{from: "", to: ""})) }
  end

  def render(assigns) do
    ~H"""
    <div>
      <h1>Jumpoverjump</h1>

      <%= @turn %>
      <%= @last_move %>
      <.simple_form for={@form} phx-submit="move">
        <.input field={@form[:from]} value={@form.params.from} />
        <.input field={@form[:to]} value={@form.params.to} />
        <.button>Move</.button>
      </.simple_form>
    </div>
    """
  end

  def handle_event("move", %{"from" => from, "to" => to}, socket) do
    new_turn = if socket.assigns.turn == :white, do: :black, else: :white
    {:noreply, assign(socket, turn: new_turn, last_move: from <> " " <> to)}
  end
end
