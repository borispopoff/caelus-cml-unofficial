if (mesh.changing())
{
    forAll(U.boundaryField(), patchi)
    {
        if (U.boundaryField()[patchi].fixesValue())
        {
            U.boundaryFieldRef()[patchi].initEvaluate();
        }
    }

    forAll(U.boundaryField(), patchi)
    {
        if (U.boundaryField()[patchi].fixesValue())
        {
            U.boundaryFieldRef()[patchi].evaluate();

            phi.boundaryFieldRef()[patchi] =
                U.boundaryField()[patchi]
              & mesh.Sf().boundaryField()[patchi];
        }
    }
}

{
    volScalarField pcorr
    (
        IOobject
        (
            "pcorr",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("pcorr", p.dimensions(), 0),
        pcorrTypes
    );

    dimensionedScalar rAUf("rAUf", dimTime, 1.0);

    while (pimple.correctNonOrthogonal())
    {
        fvScalarMatrix pcorrEqn
        (
            fvm::laplacian(rAUf, pcorr) == fvc::div(phi)
        );

        pcorrEqn.setReference(pRefCell, pRefValue);
        pcorrEqn.solve();

        if (pimple.finalNonOrthogonalIter())
        {
            phi -= pcorrEqn.flux();
        }
    }

    #include "continuityErrs.hpp"
}
