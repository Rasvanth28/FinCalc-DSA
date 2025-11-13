import java.util.Scanner;
interface IncomeTax 
{
    float baseamt = 50000f;
    float computeTax(float incomeAmt, float taxPercent);
    boolean checkEligibility(float incomeAmt);
}
class Tax_payee implements IncomeTax 
{
    String name;
    float income_amt;
    float tax_percent;
    float Taxamt;
    public Tax_payee(String name, float income_amt, float tax_percent) 
    {
        this.name = name;
        this.income_amt = income_amt;
        this.tax_percent = tax_percent;
        this.Taxamt = 0f;
    }
    @Override
    public boolean checkEligibility(float incomeAmt)
    {
        return incomeAmt > baseamt;
    }
    @Override
    public float computeTax(float incomeAmt, float taxPercent) 
    {
        if (!checkEligibility(incomeAmt)) 
        {
            return 0f;
        }
        float taxable = incomeAmt - baseamt;
        Taxamt = taxable * taxPercent / 100f; 
        return Taxamt;
    }
    public void display() 
    {
        System.out.printf("Name: %s\nIncome: %.2f\nTax Percent: %.2f%%\nTax Amount: %.2f\n",
            name, income_amt, tax_percent, Taxamt);
    }
}
public class TaxPayeeDemo 
{
    public static void main(String[] args)
    {
        Scanner sc = new Scanner(System.in);
        System.out.print("Enter name: ");
        String name = sc.nextLine();
        System.out.print("Enter income amount: ");
        float income = sc.nextFloat();
        System.out.print("Enter tax percent (e.g., 10 for 10%): ");
        float tp = sc.nextFloat();
        Tax_payee tpyee = new Tax_payee(name, income, tp);
        if (tpyee.checkEligibility(income)) 
        {
            tpyee.computeTax(income, tp);
            System.out.println("\n--- Tax Details ---");
            tpyee.display();
        } 
        else
        {
            System.out.println("\nNot eligible for tax (income <= base amount: " + IncomeTax.baseamt + ")");
            tpyee.display();
        }
        sc.close();
    }
}