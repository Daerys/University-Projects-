package info.kgeorgiy.ja.vikulaev.bank.Tests;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;
import info.kgeorgiy.ja.vikulaev.bank.Bank.Bank;
import info.kgeorgiy.ja.vikulaev.bank.Bank.RemoteBank;
import info.kgeorgiy.ja.vikulaev.bank.Person.Person;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.rmi.RemoteException;
import java.util.Random;

import static org.junit.jupiter.api.Assertions.*;

public class BankTests {

    public static final int PORT = 8080;

    public static final int LEFT_LIMIT = 97;
    public static final int RIGHT_LIMIT = 122;
    private Bank bank;
    private Person remotePerson;
    private Person localPerson;

    @BeforeEach
    public void setup() throws RemoteException {
        bank = new RemoteBank(PORT);
        remotePerson = bank.createPerson("John", "Doe", "1234567890");
        bank.createAccount("123456789", remotePerson.getPassportID());
        localPerson = bank.getLocalPerson("1234567890");
    }

    @Test
    public void test_0_checker() throws RemoteException {
        System.err.println("\ntest_0_checker\n======================\n");
        Account account = bank.getAccount("123456789");
        assertNull(account);
        account = bank.getAccount(remotePerson.fullName("123456789"));
        assertEquals(localPerson.getPassportID(), account.getPassportID());
        assertEquals(remotePerson.getPassportID(), account.getPassportID());

        System.err.println("Checking errors\n-----------------------\n");

        for (int i = 0; i < 10; ++i) {
            System.out.println();
            String firstName = new Random().ints(LEFT_LIMIT, RIGHT_LIMIT + 1)
                    .limit((int) (Math.random() * (1 << 5)))
                    .collect(StringBuilder::new, StringBuilder::appendCodePoint, StringBuilder::append)
                    .toString();
            String lastName = new Random().ints(LEFT_LIMIT, RIGHT_LIMIT + 1)
                    .limit((int) (Math.random() * (1 << 5)))
                    .collect(StringBuilder::new, StringBuilder::appendCodePoint, StringBuilder::append)
                    .toString();
            Person person = bank.createPerson(firstName, lastName, String.valueOf(new Random().nextInt(1 << 10)));
            if (person != null) {
                fail("test_0_checker fail. Wrong input.");
            }
        }
        System.err.println("=====================\n");
    }

    @Test
    public void test_1_testLocal() throws RemoteException {
        System.err.println("\ntest_1_testLocal\n======================\n");
        String accountID = "123456789";
        Account account = localPerson.getAccount(accountID);
        localPerson.changeAccountAmount(accountID, 100);
        assertEquals(account.getAmount(), localPerson.getAccount(accountID).getAmount());
        System.err.println("=====================\n");
    }

    @Test
    public void test_2_testRemote() throws RemoteException {
        System.err.println("\ntest_2_testRemote\n======================\n");
        String accountID = "123456789";
        Account account = remotePerson.getAccount(accountID);
        remotePerson.changeAccountAmount(accountID, 100);
        assertEquals(account.getAmount(), remotePerson.getAccount(accountID).getAmount());
        System.err.println("=====================\n");
    }

    @Test
    public void test_3_testLocalAndRemoteChange() throws RemoteException {
        System.err.println("\ntest_3_testLocalAndRemoteChange\n======================\n");
        String accountID = "123456789";
        remotePerson.changeAccountAmount(accountID, 100);
        assertNotEquals(remotePerson.getAccount(accountID).getAmount(), localPerson.getAccount(accountID).getAmount());
        System.err.println("=====================\n");
    }

}