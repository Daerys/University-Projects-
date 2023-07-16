package info.kgeorgiy.ja.vikulaev.bank.Account;


import java.io.Serializable;
import java.rmi.RemoteException;

public class LocalAccount extends AbstractAccount implements Serializable {
    public LocalAccount(final Account account) throws RemoteException {
        super(account.getId(), account.getPassportID(), account.getAmount());
    }

    public LocalAccount(String id, int amount, String passportId) {
        super(id, passportId, amount);
    }
}
