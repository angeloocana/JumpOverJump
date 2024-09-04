angular.module("jojApp").config(["$translateProvider", "$localStorageProvider", function ($translateProvider, $localStorageProvider) {
    $translateProvider.translations("en-us", {
        "NEW_GAME":"New Game",
        "NEW_GAME_OFFLINE": "New Game Offline",
        "NEW_GAME_VS_COMPUTER": "New Game vs Computer",
        "NEW_GAME_ONLINE": "New Game Online",

        "SWITCH_PLAYERS":"Switch players",
        "ENTER_PLAYER_NAMES":"Enter Players Names",

        "PLAY":"Play",

        "DOWNLOAD":"Download",

        "WHITE_TURN": "White turn",
        "BLACK_TURN": "Black turn",
        "BLACK_WIN": "Black Win!",
        "WHITE_WIN": "White Win!",
        "SHOW_MOVES": "Show Moves",

        "WHITE": "White",
        "BLACK": "Black",

        "WHITE_PLAYER_NAME": "White Player Name:",
        "BLACK_PLAYER_NAME": "Black Player Name:",

        "WON_POSITIONS": "Won Positions",
        "JUMPS": "Jumps",
        "N_MOVES": "Nº Moves",
        "Points": "Points",

        "BACK_MOVE": "Back Move",

        "LANGUAGE": "Language",
        "LANGUAGES_SELECT_YOUR_COUNTRY": "Choose your language.",
        "CREATE_ACCOUNT": "Create account",
        "SIGN_IN": "Sign in",
        "NOT_REGISTERED": "Not registered?"        
    });

    $translateProvider.translations("pt-br", {
        "NEW_GAME": "Novo Jogo",
        "NEW_GAME_OFFLINE": "Novo jogo Offline",
        "NEW_GAME_VS_COMPUTER": "Novo Jogo x Computador",
        "NEW_GAME_ONLINE": "Novo Jogo Online",

        "SWITCH_PLAYERS": "Inverter Jogadores",

        "ENTER_PLAYER_NAMES": "Digite o nome dos jogadores",

        "PLAY": "Jogar",

        "DOWNLOAD": "Baixar",

        "WHITE_TURN": "Vez das brancas",
        "BLACK_TURN": "Vez das pretas",
        "BLACK_WIN": "Pretas venceram",
        "WHITE_WIN": "Brancas venceram",
        "WHITE": "Branco",
        "BLACK": "Preto",

        "WHITE_PLAYER_NAME": "Nome Jogador Branco:",
        "BLACK_PLAYER_NAME": "Nome Jogador Preto:",

        "SHOW_MOVES": "Mostrar Jogadas",

        "BACK_MOVE": "Voltar Jogada",

        "WON_POSITIONS": "Posições Domindas",
        "JUMPS": "Pulos",
        "N_MOVES": "Nº Jogadas",
        "Points": "Pontos",

        "LANGUAGE": "Idioma",
                
        "CREATE_ACCOUNT": "Cadastre-se",
        "SIGN_IN": "Login",
        "NOT_REGISTERED": "Ainda não tem cadastro?"        
    });

    var getLocalLangKey = function () {
        if (window.navigator.userLanguage)
            return window.navigator.userLanguage;
        else if (window.navigator.language)
            return window.navigator.language;
        else
            return "en-us";
    }

    var getValidLangKey = function () {
        var langKey = getLocalLangKey().toLowerCase();
        switch (langKey) {
            case "pt-br":
                return "pt-br";
            default:
                return "en-us";
        }
    }

    $translateProvider.useSanitizeValueStrategy("escapeParameters");

    if (!$localStorageProvider.get("preferredLanguage")) {
        $localStorageProvider.set("preferredLanguage", getValidLangKey());
    }

    $translateProvider.preferredLanguage($localStorageProvider.get("preferredLanguage"));
}]);