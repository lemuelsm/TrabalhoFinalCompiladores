import java.io.*;
import java.util.*;
import java.lang.management.ManagementFactory;
import java.lang.management.MemoryUsage;

class Cidade {
    private int x, y;

    public Cidade(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public static int gerarNumeroAleatorio(int intervalo) {
        return new Random().nextInt(intervalo);
    }

    public static Cidade gerarCoordenadas(int intervalo) {
        return new Cidade(gerarNumeroAleatorio(intervalo), gerarNumeroAleatorio(intervalo));
    }

    public static double calcularDistancia(Cidade a, Cidade b) {
        return Math.sqrt(Math.pow(b.getX() - a.getX(), 2) + Math.pow(b.getY() - a.getY(), 2));
    }
}

class AlgoritmoForcaBruta {
    private List<Cidade> cidades;
    private double menorDistancia = Double.MAX_VALUE;
    private int[] melhorCaminho;
    private int[] caminhoAtual;

    public AlgoritmoForcaBruta(List<Cidade> cidades) {
        this.cidades = cidades;
        this.caminhoAtual = new int[cidades.size()];
        this.melhorCaminho = new int[cidades.size()];

        for (int i = 0; i < cidades.size(); i++) {
            caminhoAtual[i] = i;
        }
    }

    public void permutar(int l, int r, BufferedWriter arquivoCaminhos) throws IOException {
        if (l == r) {
            double distancia = calcularDistanciaTotal();
            long startTime = System.nanoTime();
            long endTime = System.nanoTime();
            double tempoExecucao = (endTime - startTime) / 1_000_000.0;

            StringBuilder caminho = new StringBuilder();
            for (int i = 0; i < cidades.size(); i++) {
                caminho.append(caminhoAtual[i] + 1).append(" -> ");
            }
            caminho.append(caminhoAtual[0] + 1);
            System.out.printf("Caminho testado: %s | Distancia: %.2f Km | Tempo: %.6f s\n", caminho.toString(), distancia, tempoExecucao);
            arquivoCaminhos.write(caminho.toString() + String.format(", %.2f, %.6f\n", distancia, tempoExecucao));

            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                System.arraycopy(caminhoAtual, 0, melhorCaminho, 0, cidades.size());
            }
        } else {
            for (int i = l; i <= r; i++) {
                trocar(l, i);
                permutar(l + 1, r, arquivoCaminhos);
                trocar(l, i);
            }
        }
    }

    private void trocar(int i, int j) {
        int temp = caminhoAtual[i];
        caminhoAtual[i] = caminhoAtual[j];
        caminhoAtual[j] = temp;
    }

    private double calcularDistanciaTotal() {
        double distanciaTotal = 0.0;
        for (int i = 0; i < cidades.size() - 1; i++) {
            distanciaTotal += Cidade.calcularDistancia(cidades.get(caminhoAtual[i]), cidades.get(caminhoAtual[i + 1]));
        }
        distanciaTotal += Cidade.calcularDistancia(cidades.get(caminhoAtual[cidades.size() - 1]), cidades.get(caminhoAtual[0]));
        return distanciaTotal;
    }

    public void executar() {
        try (BufferedWriter arquivoCaminhos = new BufferedWriter(new FileWriter("caminhos.txt"))) {
            long startTime = System.nanoTime();
            MemoryUsage heapBefore = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage();
            long memoriaAntes = heapBefore.getUsed();

            permutar(0, cidades.size() - 1, arquivoCaminhos);

            MemoryUsage heapAfter = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage();
            long memoriaDepois = heapAfter.getUsed();
            long endTime = System.nanoTime();

            double tempoExecucaoTotal = (endTime - startTime) / 1_000_000_000.0;
            long memoriaUsada = (memoriaDepois - memoriaAntes) / 1024; // Convertendo para KB

            System.out.println("\nMelhor caminho encontrado:");
            for (int i = 0; i < cidades.size(); i++) {
                System.out.print((melhorCaminho[i] + 1) + " -> ");
            }
            System.out.println(melhorCaminho[0] + 1);
            System.out.printf("Distancia total: %.2f Km\n", menorDistancia);
            System.out.printf("Tempo total de execução: %.6f segundos\n", tempoExecucaoTotal);
            System.out.printf("Memória usada: %d KB\n", memoriaUsada);

            salvarMelhorCaminho();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void salvarMelhorCaminho() {
        try (BufferedWriter arquivoCaminhoMinimo = new BufferedWriter(new FileWriter("caminhominimo.txt"))) {
            arquivoCaminhoMinimo.write("Melhor caminho:\n");
            for (int i = 0; i < cidades.size() - 1; i++) {
                double distancia = Cidade.calcularDistancia(cidades.get(melhorCaminho[i]), cidades.get(melhorCaminho[i + 1]));
                arquivoCaminhoMinimo.write(String.format("%d -> %d: %.2f Km\n", melhorCaminho[i] + 1, melhorCaminho[i + 1] + 1, distancia));
            }
            double distanciaFinal = Cidade.calcularDistancia(cidades.get(melhorCaminho[cidades.size() - 1]), cidades.get(melhorCaminho[0]));
            arquivoCaminhoMinimo.write(String.format("%d -> %d: %.2f Km\n", melhorCaminho[cidades.size() - 1] + 1, melhorCaminho[0] + 1, distanciaFinal));
            arquivoCaminhoMinimo.write(String.format("Distancia total: %.2f Km\n", menorDistancia));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

public class Main {
    public static List<Cidade> gerarCidades(int quantidade, int intervalo) {
        List<Cidade> cidades = new ArrayList<>();
        Set<String> coordenadasUnicas = new HashSet<>();
        int geradas = 0;

        while (geradas < quantidade) {
            Cidade novaCidade = Cidade.gerarCoordenadas(intervalo);
            String coordenada = novaCidade.getX() + "," + novaCidade.getY();

            if (!coordenadasUnicas.contains(coordenada)) {
                cidades.add(novaCidade);
                coordenadasUnicas.add(coordenada);
                geradas++;
            }
        }
        return cidades;
    }

    public static List<Cidade> carregarCidadesDeArquivo() {
        List<Cidade> cidades = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader("cidades.txt"))) {
            String linha;
            while ((linha = br.readLine()) != null) {
                String[] partes = linha.replace("Cidade", "").replace("(", "").replace(")", "").split(":");
                String[] coords = partes[1].split(",");
                int x = Integer.parseInt(coords[0].trim());
                int y = Integer.parseInt(coords[1].trim());
                cidades.add(new Cidade(x, y));
            }
        } catch (IOException e) {
            System.out.println("Erro ao carregar cidades.");
        }
        return cidades;
    }

    public static void salvarCidades(List<Cidade> cidades) {
        try (BufferedWriter arquivo = new BufferedWriter(new FileWriter("cidades.txt"))) {
            for (int i = 0; i < cidades.size(); i++) {
                arquivo.write(String.format("Cidade %d: (%d, %d)\n", i + 1, cidades.get(i).getX(), cidades.get(i).getY()));
            }
        } catch (IOException e) {
            System.out.println("Erro ao salvar cidades.");
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        List<Cidade> cidades = new ArrayList<>();
        int escolha, quantidade, intervalo = 100;

        System.out.print("Deseja gerar um novo conjunto de cidades (1) ou carregar de um arquivo existente (2)? ");
        escolha = scanner.nextInt();

        if (escolha == 1) {
            System.out.print("Quantas cidades deseja gerar? (max: 8)");
            quantidade = scanner.nextInt();
            cidades = gerarCidades(quantidade, intervalo);
            salvarCidades(cidades);
        } else if (escolha == 2) {
            cidades = carregarCidadesDeArquivo();
            if (cidades.isEmpty()) {
                System.out.println("Erro ao carregar cidades.");
                return;
            }
        } else {
            System.out.println("Escolha inválida.");
            return;
        }

        AlgoritmoForcaBruta algoritmo = new AlgoritmoForcaBruta(cidades);
        algoritmo.executar();
    }
}
