defmodule Jumpoverjump.Repo do
  use Ecto.Repo,
    otp_app: :jumpoverjump,
    adapter: Ecto.Adapters.Postgres
end
