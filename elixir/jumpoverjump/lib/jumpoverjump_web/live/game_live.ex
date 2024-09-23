defmodule JumpoverjumpWeb.GameLive do
  use JumpoverjumpWeb, :live_view

  @board_last_index 7

  def mount(_params, _session, socket) do
    {:ok, assign(socket,
      turn: :white,
      last_move: "",
      form: to_form(%{from: "", to: ""}),
      board: initialize_board()
    )}
  end

  def handle_event("move", %{"from" => from, "to" => to}, socket) do
    new_turn = if socket.assigns.turn == :white, do: :black, else: :white
    {:noreply, assign(socket, turn: new_turn, last_move: from <> " " <> to)}
  end

  defp initialize_board do
    for i <- 0..@board_last_index do
      for _j <- 0..@board_last_index do
        cond do
          i == 0 -> 1
          i == @board_last_index -> 2
          true -> 0
        end
      end
    end
  end
end
