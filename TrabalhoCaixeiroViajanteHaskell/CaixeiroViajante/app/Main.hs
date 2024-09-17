import System.Random (randomRIO)
import Control.Monad (replicateM)
import Data.List (permutations)
import Text.Printf (printf)
import System.CPUTime (getCPUTime)
import Control.DeepSeq (deepseq)
import GHC.Stats (getRTSStatsEnabled, getRTSStats, max_live_bytes)
import qualified Data.ByteString.Lazy.Char8 as B

-- Tipo de dados para representar uma cidade com coordenadas x e y
data Cidade = Cidade { x :: Int, y :: Int } deriving (Show, Eq)

-- Função para gerar um número aleatório dentro de um intervalo
gerarNumeroAleatorio :: Int -> IO Int
gerarNumeroAleatorio intervalo = randomRIO (0, intervalo)

-- Função para gerar uma cidade com coordenadas aleatórias
gerarCidade :: Int -> IO Cidade
gerarCidade intervalo = do
    xCoord <- gerarNumeroAleatorio intervalo
    yCoord <- gerarNumeroAleatorio intervalo
    return $ Cidade xCoord yCoord

-- Função para gerar uma lista de cidades únicas
gerarCidades :: Int -> Int -> IO [Cidade]
gerarCidades quantidade intervalo = do
    let gerarUnica cidades = do
          nova <- gerarCidade intervalo
          if nova `elem` cidades then gerarUnica cidades else return nova
    replicateM quantidade (gerarUnica [])

-- Função para calcular a distância euclidiana entre duas cidades
calcularDistancia :: Cidade -> Cidade -> Double
calcularDistancia (Cidade x1 y1) (Cidade x2 y2) =
    sqrt $ fromIntegral ((x2 - x1) ^ 2 + (y2 - y1) ^ 2)

-- Função para calcular a distância total de um caminho
calcularDistanciaTotal :: [Cidade] -> [Int] -> Double
calcularDistanciaTotal cidades caminho =
    sum $ zipWith calcularDistancia (map (cidades !!) caminho) (map (cidades !!) (tail caminho ++ [head caminho]))

-- Função para obter tempo em segundos
tempoEmSegundos :: IO Double
tempoEmSegundos = do
    t <- getCPUTime
    return (fromIntegral t / (10^12))

-- Função para permutar e testar todos os caminhos, mostrando os resultados
permutarCaminhos :: [Cidade] -> [Int] -> IO ([Int], Double)
permutarCaminhos cidades caminho = do
    let perms = permutations caminho
    testarCaminhos cidades perms (head perms, calcularDistanciaTotal cidades (head perms))

-- Função para testar todos os caminhos e imprimir cada um deles
testarCaminhos :: [Cidade] -> [[Int]] -> ([Int], Double) -> IO ([Int], Double)
testarCaminhos cidades [] melhorCaminho = return melhorCaminho
testarCaminhos cidades (c:cs) (melhorCaminho, menorDistancia) = do
    let distanciaAtual = calcularDistanciaTotal cidades c
    deepseq distanciaAtual $ do
        -- Imprime o caminho e a distância correspondente
        printf "Caminho: %s -> Distância: %.2f Km\n" (show $ map (+1) c) distanciaAtual
        if distanciaAtual < menorDistancia
          then testarCaminhos cidades cs (c, distanciaAtual)
          else testarCaminhos cidades cs (melhorCaminho, menorDistancia)

-- Função para calcular e exibir o uso de memória em KB
obterUsoDeMemoria :: IO ()
obterUsoDeMemoria = do
    statsEnabled <- getRTSStatsEnabled
    if statsEnabled
    then do
        stats <- getRTSStats
        let memoriaEmBytes = max_live_bytes stats
        let memoriaEmKB = fromIntegral memoriaEmBytes / 1024 :: Double
        printf "Memória máxima usada: %.2f KB\n" memoriaEmKB
    else
        putStrLn "As estatísticas de uso de memória não estão habilitadas. Execute com '+RTS -T'."

-- Função principal para rodar o algoritmo de força bruta
algoritmoForcaBruta :: [Cidade] -> IO ()
algoritmoForcaBruta cidades = do
    let caminhoInicial = [0..(length cidades - 1)]
    
    -- Obtém o tempo inicial
    inicio <- tempoEmSegundos

    -- Permuta e encontra o melhor caminho
    (melhorCaminho, menorDistancia) <- permutarCaminhos cidades caminhoInicial

    -- Obtém o tempo final
    fim <- tempoEmSegundos

    -- Exibe o resultado
    putStrLn "Melhor caminho (Forca Bruta):"
    print $ map (+1) melhorCaminho
    printf "Distancia total: %.2f Km\n" menorDistancia
    printf "Tempo total de execucao: %.6f segundos\n" (fim - inicio)

    -- Exibe o uso de memória
    obterUsoDeMemoria

-- Função para carregar cidades de um arquivo
carregarCidadesDeArquivo :: FilePath -> IO [Cidade]
carregarCidadesDeArquivo caminho = do
    conteudo <- B.readFile caminho
    let cidades = map (\linha -> let [x, y] = map read (words linha) in Cidade x y) (lines (B.unpack conteudo))
    return cidades

-- Função principal
main :: IO ()
main = do
    putStrLn "Deseja gerar um novo conjunto de cidades (1) ou carregar de um arquivo existente (2)?"
    escolha <- readLn
    cidades <- case escolha of
        1 -> do
            putStrLn "Quantas cidades deseja gerar? (max: 9)"
            quantidade <- readLn
            cidadesGeradas <- gerarCidades quantidade 100
            -- Salva as cidades no arquivo
            writeFile "cidades.txt" (unlines $ map (\(Cidade x y) -> show x ++ " " ++ show y) cidadesGeradas)
            return cidadesGeradas
        2 -> carregarCidadesDeArquivo "cidades.txt"
        _ -> error "Escolha invalida"

    -- Executa o algoritmo de força bruta
    algoritmoForcaBruta cidades
