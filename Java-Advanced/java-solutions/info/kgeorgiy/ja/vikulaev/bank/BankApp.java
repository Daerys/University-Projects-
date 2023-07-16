package info.kgeorgiy.ja.vikulaev.bank;

import info.kgeorgiy.ja.vikulaev.bank.Bank.Bank;
import info.kgeorgiy.ja.vikulaev.bank.Bank.RemoteBank;
import info.kgeorgiy.ja.vikulaev.bank.Person.Person;

import java.rmi.RemoteException;
import java.util.Scanner;

/**
 * The {@code BankApp} class is a simple command-line application for interacting with a remote bank.
 * It allows registering persons, creating accounts, and performing various operations on the bank.
 */
public class BankApp {
    /**
     * The entry point of the application.
     *
     * @param args command-line arguments
     */
    public static void main(String[] args) {
        BankApp app = new BankApp();
        app.run();
    }

    /**
     * Runs the application, allowing the user to interact with the remote bank through the command-line interface.
     */
    public void run() {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Enter the bank's port: ");

        Bank bank;
        while (true) {
            try {
                int port = Integer.parseInt(scanner.nextLine());
                bank = new RemoteBank(port);
                break;
            } catch (NumberFormatException ignore) {
                System.out.println("Invalid port. Enter the bank's port (numeric value): ");
            }
        }

        System.out.println("Enter a command or use 'help' to see list of commands.");
        while (true) {
            String command = scanner.nextLine();
            if (command.equals("exit")) {
                return;
            }
            if (command.equals("help")) {
                printHelp();
                continue;
            }
            if (parseCommand(command, bank)) {
                continue;
            }
            System.out.println("Incorrect input. Use 'help' to see list of commands");
        }

    }

    /**
     * Parses and executes the user command.
     *
     * @param command the command to parse
     * @param bank    the remote bank object
     * @return {@code true} if the command was successfully parsed and executed, {@code false} otherwise
     */
    private boolean parseCommand(String command, final Bank bank) {
        String[] args = command.split("\\s+");
        if (args.length < 2 || args.length > 5) {
            return false;
        }
        for (String arg : args) {
            if (arg == null) {
                return false;
            }
        }
        if (args.length == 2) {
            try {
                bank.createAccount(args[1], args[0]);
                return true;
            } catch (RemoteException e) {
                System.err.println(e.getMessage());
                return false;
            } catch (NullPointerException e) {
                return false;
            }
        }
        if (args.length == 3) {
            Person person = bank.getRemotePerson(args[0]);
            if (person != null) {
                try {
                    bank.getAccount(person.fullName(args[1])).setAmount(Integer.parseInt(args[2]));
                    return true;
                } catch (RemoteException e) {
                    System.err.println(e.getMessage());
                    return false;
                } catch (IllegalArgumentException e) {
                    System.err.println("Amount must be numeric.");
                    return false;
                }
            }

            person = bank.createPerson(args[0], args[1], args[2]);
            if (person == null) {
                return false;
            }
        }
        if (args.length == 5) {
            Person person = bank.createPerson(args[0], args[1], args[2]);
            if (person == null) {
                return false;
            }
            try {
                bank.getAccount(person.fullName(args[3])).setAmount(Integer.parseInt(args[4]));
            } catch (RemoteException e) {
                System.err.println(e.getMessage());
                return false;
            } catch (NumberFormatException e) {
                System.err.println("Amount must be numeric.");
                return false;
            }
        }
        return true;
    }

    /**
     * Prints the list of available commands.
     */
    private void printHelp() {
        System.out.println("""
                Here is a list of commands which you can use:
                    1) <person firstname> <person lastname> <person passportID> <person account number> <amount> | To register a person, account and set money
                    2) <person firstname> <person lastname> <person passportID>                                  | To register a person
                    3) <person passportID> <person account number>                                               | To make an account
                    4) <person passportID> <person account number> <amount>                                      | To make an account and/or set/add money
                """);
    }
}
